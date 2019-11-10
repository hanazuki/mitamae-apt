module MItamae
  module InlineBackends
    class AptBackend
      def runnable?(type, *)
        respond_to?(type)
      end

      def run(type, *arg)
        send(type, *arg)
      end

      def check_package_is_installed(name, version = nil)
        AptPkg.installed?(name, version)
      end

      def get_package_version(name)
        if version = AptPkg.installed_version(name)
          Specinfra::CommandResult.new(stdout: version, stderr: '', exit_status: 0)
        else
          Specinfra::CommandResult.new(stdout: '', stderr: "#{name} is not installed", exit_status: 1)
        end
      end
    end
  end
end
