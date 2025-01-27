{ pkgs ? import <nixpkgs> {} }:
pkgs.pkgsCross.riscv64.mkShell {
  depsBuildBuild = with pkgs; [
    gcc
    gnumake
    patchelf

    # LaTeX
    (texliveBasic.withPackages (ps: with ps; [
      collection-latexrecommended
      collection-fontsrecommended
      collection-latexextra
      collection-plaingeneric
      collection-langcjk
      collection-binextra
      collection-bibtexextra
    ]))
  ];

  SIMICS_BASE = "${builtins.getEnv "HOME"}/Software/simics/simics/simics-7.21.0/";
}
