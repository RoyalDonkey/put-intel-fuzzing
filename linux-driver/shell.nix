let
  system = "x86_64-linux";
  pkgs = import <nixpkgs> {};
in (pkgs.buildFHSEnv {
    name = "linux-fhs";
    targetPkgs = pkgs: with pkgs; [
      gcc
      gnumake
      file
      ncurses ncurses.dev
      pkg-config
      dtc
    ];
  }).env
