.PHONY: graphplay graphplay-test clean

graphplay:
	$(MAKE) -C graphplay

graphplay-test:
	$(MAKE) -C graphplay-test

clean:
	$(MAKE) -C graphplay clean
	$(MAKE) -C graphplay-test clean
	rm -f gmon.out
