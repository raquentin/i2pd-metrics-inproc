{
  description = "i2pd-metrics development environment";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = nixpkgs.legacyPackages.${system};
      in
      {
        devShells.default = pkgs.mkShell {
          buildInputs = with pkgs; [
            gcc
            gnumake
            boost
            openssl
            zlib
            curl
          ];

          shellHook = ''
            echo "i2pd-metrics development environment"
            echo "Boost version: ${pkgs.boost.version}"
          '';
        };
      }
    );
}
