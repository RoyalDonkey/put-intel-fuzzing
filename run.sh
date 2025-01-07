#/bin/sh

# Runs the bad program with preloaded libsan-overlay.
# https://github.com/google/sanitizers/wiki/AddressSanitizerFlags

BAD_PROGRAM=bad
LIBSAN_OVERLAY=libsan-overlay.so

[ ! -f "$BAD_PROGRAM" ] && echo "'$BAD_PROGRAM' program missing; did you forget to run make?" >&2 && exit 1
[ ! -f "$LIBSAN_OVERLAY" ] && echo "'$LIBSAN_OVERLAY' library missing; did you forget to run make?" >&2 && exit 1


echo "[PRELOAD SANITY CHECK] ldd $BAD_PROGRAM:"
ASAN_OPTIONS='verify_asan_link_order=0' LD_PRELOAD="./$LIBSAN_OVERLAY" ldd $BAD_PROGRAM
echo

ASAN_OPTIONS='verify_asan_link_order=0' LD_PRELOAD="./$LIBSAN_OVERLAY" ./$BAD_PROGRAM
