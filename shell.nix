{ pkgs ? import <nixpkgs-unstable> {} }:

pkgs.mkShell {
  buildInputs = [
    pkgs.go
    pkgs.python310
    pkgs.python310Packages.pip
    pkgs.python310Packages.virtualenv
    pkgs.nodePackages.pyright
    pkgs.gopls
    pkgs.plantuml
    pkgs.texliveFull
    pkgs.python310Packages.pygments

    # keep this line if you use bash
    pkgs.bashInteractive
  ];
}
