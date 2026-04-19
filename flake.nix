{
  description = "Prometheus/OpenMetrics metrics endpoint for i2pd";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = {
    self,
    nixpkgs,
    flake-utils,
  }: let
    nixosModule = {
      config,
      lib,
      pkgs,
      ...
    }:
      with lib; let
        cfg = config.services.i2pd.metrics;
      in {
        options.services.i2pd.metrics = {
          enable = mkEnableOption "Prometheus/OpenMetrics endpoint for i2pd";

          address = mkOption {
            type = types.str;
            default = "127.0.0.1";
            description = "Address to bind the metrics endpoint to";
          };

          port = mkOption {
            type = types.port;
            default = 9600;
            description = "Port for the metrics endpoint";
          };
        };

        config = mkIf cfg.enable {
          services.i2pd = {
            enable = true;
            package = self.packages.${pkgs.system}.i2pd-with-metrics;
            extraConfig = ''
              [metrics]
              enabled = true
              address = ${cfg.address}
              port = ${toString cfg.port}
            '';
          };
        };
      };

    # overlay to add i2pd-with-metrics to pkgs
    overlay = final: prev: {
      i2pd-with-metrics = self.packages.${final.system}.i2pd-with-metrics;
    };
  in
    flake-utils.lib.eachDefaultSystem (
      system: let
        pkgs = nixpkgs.legacyPackages.${system};

        # generate the patch
        patchFile =
          pkgs.runCommand "i2pd-metrics.patch" {
            nativeBuildInputs = [pkgs.git pkgs.python3];
          } ''
            cp -r ${./.} /build/source
            cd /build/source
            chmod -R +w .
            bash generate-patch.sh
            cp i2pd-metrics.patch $out
          '';

        # i2pd with metrics patch applied
        i2pd-with-metrics = pkgs.i2pd.overrideAttrs (oldAttrs: {
          pname = "i2pd-with-metrics";
          patches = (oldAttrs.patches or []) ++ [patchFile];

          meta =
            oldAttrs.meta
            // {
              description = "i2pd with Prometheus/OpenMetrics endpoint";
              longDescription = ''
                i2pd (Invisible Internet Project daemon) with integrated
                Prometheus/OpenMetrics metrics endpoint for monitoring.

                Metrics available at http://127.0.0.1:9600/metrics by default.
              '';
            };
        });
      in {
        packages = {
          default = i2pd-with-metrics;
          i2pd-with-metrics = i2pd-with-metrics;
          patch = patchFile;
        };

        apps = {
          default = {
            type = "app";
            program = toString (pkgs.writeShellScript "show-patch" ''
              echo "i2pd Metrics Patch"
              echo "=================="
              echo ""
              ${pkgs.coreutils}/bin/cat ${patchFile}
            '');
          };

          show-patch = {
            type = "app";
            program = toString (pkgs.writeShellScript "show-patch" ''
              echo "i2pd Metrics Patch"
              echo "=================="
              echo ""
              ${pkgs.coreutils}/bin/cat ${patchFile}
            '');
          };
        };

        devShells.default = pkgs.mkShell {
          buildInputs = with pkgs; [
            gcc
            gnumake
            boost
            openssl
            zlib
            curl
            git
            python3
          ];

          shellHook = ''
            echo "i2pd-metrics development environment"
            echo "Boost version: ${pkgs.boost.version}"
            echo ""
            echo "Commands:"
            echo "  make         - Build metrics objects"
            echo "  make test    - Run unit tests"
            echo "  ./generate-patch.sh - Generate patch for i2pd"
          '';
        };
      }
    )
    // {
      nixosModules.default = nixosModule;
      overlays.default = overlay;
    };
}
