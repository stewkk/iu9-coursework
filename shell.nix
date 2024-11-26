{ pkgs ? import <nixpkgs> {} }:

let
  tex = (pkgs.texlive.combine {
    inherit (pkgs.texlive) scheme-full
      latexmk;
  });
in
pkgs.mkShell.override {stdenv = pkgs.llvmPackages_18.stdenv;} {
  buildInputs = [
    pkgs.pandoc
    tex
    pkgs.python312Packages.pygments
    pkgs.cmake

    # keep this line if you use bash
    pkgs.bashInteractive
  ];

  nativeBuildInputs = [
    pkgs.clang-tools_18
  ];
}

