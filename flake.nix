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
          qotd-8ball = pkgs.stdenv.mkDerivation {
            pname = "qotd-8ball";
            version = "1.0.0";
            src = ./.;
            
            buildInputs = [ ];
            nativeBuildInputs = [ pkgs.gcc ];
            
            buildPhase = ''
              gcc -o qotd-8ball qotd_8ball.c
            '';
            
            installPhase = ''
              mkdir -p $out/bin
              cp qotd-8ball $out/bin/
            '';
          };

          qotd-fortune-cowsay = pkgs.stdenv.mkDerivation {
            pname = "qotd-fortune-cowsay";
            version = "1.0.0";
            src = ./.;
            
            buildInputs = [ pkgs.fortune pkgs.cowsay ];
            nativeBuildInputs = [ pkgs.gcc ];
            
            buildPhase = ''
              gcc -o qotd-fortune-cowsay qotd_fortune.c
            '';
            
            installPhase = ''
              mkdir -p $out/bin
              cp qotd-fortune-cowsay $out/bin/
            '';
          };
        };

        devShells.default = pkgs.mkShell {
          buildInputs = [ pkgs.gcc pkgs.fortune pkgs.cowsay ];
        };
      });
}