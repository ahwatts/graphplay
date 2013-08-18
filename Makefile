.PHONY: ALL graphplay graphplay-test clean

ALL: graphplay graphplay-test

graphplay:
	$(MAKE) -C graphplay

graphplay-test:
	$(MAKE) -C graphplay-test

clean:
	$(MAKE) -C graphplay clean
	$(MAKE) -C graphplay-test clean
	rm -f gmon.out
