{
  description = "RFC865 Quote of the Day Server";

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
        packages = {
          qotd-server = pkgs.stdenv.mkDerivation {
            pname = "qotd-server";
            version = "1.0.0";
            src = ./.;
            
            buildInputs = [ pkgs.fortune pkgs.cowsay pkgs.docker ];
            nativeBuildInputs = [ pkgs.gcc ];
            
            buildPhase = ''
              cd src
              gcc -o qotd-server *.c
            '';
            
            installPhase = ''
              mkdir -p $out/bin
              cp qotd-server $out/bin/
            '';
          };
        };

        devShells.default = pkgs.mkShell {
          buildInputs = [ pkgs.gcc pkgs.fortune pkgs.cowsay pkgs.docker ];
        };
      });
}