all:
install.sql: install.sql.in
	sed 's|PG_LIB_DIR|${libdir}|g' install.sql.in | sed 's|PACKAGE|${PACKAGE}|g' > install.sql
clean: 
	rm -f install.sql

distdir = ${datadir}/cgi_pgext
dist_DATA = install.sql

