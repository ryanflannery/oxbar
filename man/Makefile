MAN_PAGES=oxbar.1

.PHONY: lint

.DEFAULT:
	@echo Nothing to do here

oxbar.html: oxbar.1
	mandoc -T html -O toc oxbar.1 > $@

lint:
	mandoc -Tlint $(MAN_PAGES)

test:
	mandoc oxbar.1 | less
