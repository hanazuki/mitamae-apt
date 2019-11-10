# mitamae-apt

`mitamae-apt` mrbgem provides `MItamae::InlineBackends::AptBackend` which makes `mitamae`'s package management faster.

*Remark:* To improve the performance of retrieving package installation status, `mitamae-apt` reads the package cache maintained by APT instead of querying dpkg as `mitamae` does by default. This APT's package cache is a succinct view of dpkg's status database, but it is solely updated by APT and dpkg knows nothing about it. While you activate `mitamae-apt` do not install or remove packages directly using `dpkg` command, or `mitame` may be confused by a stale package cache. Anyway, executing `apt-get update` will always rebuild the cache from the original status database.

