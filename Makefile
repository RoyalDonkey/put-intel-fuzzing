all:
	$(MAKE) -C bad-program
	$(MAKE) -C libsan-overlay

clean:
	$(MAKE) -C bad-program clean
	$(MAKE) -C libsan-overlay clean
