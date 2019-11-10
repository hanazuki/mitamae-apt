#include <apt-pkg/cachefile.h>
#include <apt-pkg/configuration.h>
#include <apt-pkg/init.h>
#include <apt-pkg/pkgcache.h>
#include <apt-pkg/pkgsystem.h>
#include <apt-pkg/versionmatch.h>
#include <mruby.h>

#include <time.h>
#include <sys/stat.h>

#include <memory>

std::shared_ptr<pkgCacheFile> cache_file;
time_t cache_timestamp;

static std::tuple<std::string, std::string> const parse_package_name(std::string const &name)
{
   auto const colon = name.find(':');

   if (colon == std::string::npos)
   {
      return std::make_tuple(name, "native");
   }

   return std::make_tuple(name.substr(0, colon), name.substr(colon + 1));
}

pkgCache *get_pkg_cache() {
  std::string const cache_path = _config->FindFile("Dir::Cache::pkgcache");

  struct stat statbuf;
  if(stat(cache_path.c_str(), &statbuf) != 0) {  // TODO: fetch path from _config
    return 0;
  }

  if(!cache_file || cache_timestamp < statbuf.st_mtime) {
    cache_file = std::make_shared<pkgCacheFile>();
    cache_timestamp = statbuf.st_mtime;
  }

  return cache_file->GetPkgCache();
}

static mrb_value mrb_apt_pkg_installed_p(mrb_state *mrb, mrb_value self)
{
   char *pname, *pversion;
   int argc = mrb_get_args(mrb, "z|z!", &pname, &pversion);
   if (argc < 2)
      pversion = 0;

   auto cache = get_pkg_cache();

   auto const pkg_name = parse_package_name(pname);
   auto pkg = cache->FindPkg(std::get<0>(pkg_name), std::get<1>(pkg_name));
   if (pkg == cache->PkgEnd())
   {
      return mrb_false_value();
   }

   auto const state = pkg->SelectedState;
   auto const installed = state == pkgCache::State::Install || state == pkgCache::State::Hold;
   if (!installed)
   {
      return mrb_false_value();
   }

   if (!pversion)
   {
      return mrb_true_value();
   }

   pkgVersionMatch match(pversion, pkgVersionMatch::Version);
   return mrb_bool_value(match.VersionMatches(pkg.CurrentVer()));
}

static mrb_value mrb_apt_pkg_installed_version(mrb_state *mrb, mrb_value self)
{
   char *pname;
   mrb_get_args(mrb, "z", &pname);

   auto cache = get_pkg_cache();

   auto const pkg_name = parse_package_name(pname);
   auto pkg = cache->FindPkg(std::get<0>(pkg_name), std::get<1>(pkg_name));
   if (pkg == cache->PkgEnd())
   {
      return mrb_nil_value();
   }

   return mrb_str_new_cstr(mrb, pkg.CurrentVer().VerStr());
}

extern "C"
{
   void mrb_mitamae_apt_gem_init(mrb_state *mrb)
   {
      pkgInitConfig(*_config);
      pkgInitSystem(*_config, _system);

      struct RClass *mod_apt_pkg = mrb_define_module(mrb, "AptPkg");
      mrb_define_singleton_method(mrb, reinterpret_cast<RObject *>(mod_apt_pkg),
				  "installed?", mrb_apt_pkg_installed_p, MRB_ARGS_ARG(1, 1));
      mrb_define_singleton_method(mrb, reinterpret_cast<RObject *>(mod_apt_pkg),
				  "installed_version", mrb_apt_pkg_installed_version, MRB_ARGS_REQ(1));
   }

   void mrb_mitamae_apt_gem_final(mrb_state *mrb)
   {
   }
}
