## Process this file with automake to produce Makefile.in


SPEC			= $(PACKAGE_NAME).spec

TARFILE			= $(PACKAGE_NAME)-$(VERSION).tar.gz

EXTRA_DIST		= autogen.sh

AUTOMAKE_OPTIONS	= foreign

MAINTAINERCLEANFILES	= Makefile.in aclocal.m4 configure depcomp \
			  config.guess config.sub missing install-sh \
			  autoheader automake autoconf test_lense.sh

dist_doc_DATA		= README COPYING

SUBDIRS			= src test

ACLOCAL_AMFLAGS		= -I m4

coverity:
	cov-build --dir=cov make
	cov-analyze --dir cov --concurrency --wait-for-license
	cov-format-errors --dir cov

install-exec-local:
	$(INSTALL) -d $(DESTDIR)/${btdsconfdir}
	$(INSTALL) -d $(DESTDIR)/${btdsarbitratordir}
	$(INSTALL) -d $(DESTDIR)/${btdssitedir}
	$(INSTALL) -d $(DESTDIR)/${SOCKETDIR}

install-exec-hook:
	ln -sf ${sbindir}/btds $(DESTDIR)/${sbindir}/btds

uninstall-local:
	rmdir $(DESTDIR)/${btdsconfdir} || :;
	rmdir $(DESTDIR)/${btdsarbitratordir} || :;
	rmdir $(DESTDIR)/${btdssitedir} || :;
	rmdir $(DESTDIR)/${SOCKETDIR} || :;

test: check

lint:
	for dir in src; do make -C $$dir lint; done

dist-clean-local:
	rm -f autoconf automake autoheader

dist-hook:
	echo $(VERSION) > $(distdir)/.tarball-version


## make rpm/srpm section.

$(SPEC): $(SPEC).in
	rm -f $@-t $@
	date="$(shell LC_ALL=C date "+%a %b %d %Y")" && \
	if [ -f .tarball-version ]; then \
		gitver="$(shell cat .tarball-version)" && \
		rpmver=$$gitver && \
		alphatag="" && \
		dirty="" && \
		numcomm="0"; \
	else \
		gitver="$(shell git describe --tags --abbrev=4 --match='v*' HEAD 2>/dev/null)" && \
		rpmver=`echo $$gitver | sed -e "s/^v//" -e "s/-.*//g"` && \
		alphatag=`echo $$gitver | sed -e "s/.*-//" -e "s/^g//"` && \
		vtag=`echo $$gitver | sed -e "s/-.*//g"` && \
		numcomm=`git rev-list $$vtag..HEAD | wc -l` && \
		git update-index --refresh > /dev/null 2>&1 || true && \
		dirty=`git diff-index --name-only HEAD 2>/dev/null`; \
	fi && \
	if [ -n "$$dirty" ]; then dirty="dirty"; else dirty=""; fi && \
	if [ "$$numcomm" = "0" ]; then \
		sed \
			-e "s#@version@#$$rpmver#g" \
			-e "s#%glo.*alpha.*##g" \
			-e "s#%glo.*numcomm.*##g" \
			-e "s#@dirty@#$$dirty#g" \
			-e "s#@date@#$$date#g" \
		$< > $@-t; \
	else \
		sed \
			-e "s#@version@#$$rpmver#g" \
			-e "s#@alphatag@#$$alphatag#g" \
			-e "s#@numcomm@#$$numcomm#g" \
			-e "s#@dirty@#$$dirty#g" \
			-e "s#@date@#$$date#g" \
		$< > $@-t; \
	fi; \
	if [ -z "$$dirty" ]; then sed -i -e "s#%glo.*dirty.*##g" $@-t; fi
	chmod a-w $@-t
	mv $@-t $@

$(TARFILE):
	$(MAKE) dist

RPMBUILDOPTS	= --define "_sourcedir $(abs_builddir)" \
		  --define "_specdir $(abs_builddir)" \
		  --define "_builddir $(abs_builddir)" \
		  --define "_srcrpmdir $(abs_builddir)" \
		  --define "_rpmdir $(abs_builddir)"

srpm: clean
	autoreconf -if
	$(MAKE) $(SPEC) $(TARFILE)
	rpmbuild $(RPMBUILDOPTS) --nodeps -bs $(SPEC)

rpm: clean
	autoreconf -if
	$(MAKE) $(SPEC) $(TARFILE)
	rpmbuild $(RPMBUILDOPTS) -ba $(SPEC)
