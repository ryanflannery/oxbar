PREFIX ?= /usr/local
MANDIR ?= $(PREFIX)/man/man1

MAN_PAGES = oxbar.1

.PHONY: lint test

.DEFAULT:
	@echo Nothing to do here

all: oxbar.html

oxbar.html: oxbar.1
	mandoc -T html -O toc oxbar.1 > $@

lint:
	mandoc -Tlint $(MAN_PAGES)

test:
	mandoc oxbar.1 | less

install:
	install -m 0444 $(MAN_PAGES) $(MANDIR)

uninstall:
	cd $(MANDIR)
	rm -f $(MAN_PAGES)
