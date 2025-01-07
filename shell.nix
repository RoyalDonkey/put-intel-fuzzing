let
  pkgs = import <nixpkgs> {};
in pkgs.mkShell {
    buildInputs = with pkgs; [
      gcc
      gnumake
    ];

    SIMICS_BASE = "${builtins.getEnv "HOME"}/Software/simics/simics/simics-7.21.0/";
  }
