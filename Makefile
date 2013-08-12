.PHONY: graphplay clean

graphplay:
	$(MAKE) -C graphplay

clean:
	$(MAKE) -C graphplay clean
	rm -f gmon.out
