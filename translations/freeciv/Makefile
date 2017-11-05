# Makefile for program source directory in GNU NLS utilities package.
# Copyright (C) 1995-1997, 2000, 2001 by Ulrich Drepper <drepper@gnu.ai.mit.edu>
#
# This file file be copied and used freely without restrictions.  It can
# be used in projects which are not available under the GNU Public License
# but which still want to provide support for the GNU gettext functionality.
# Please note that the actual code is *not* freely available.

PACKAGE = freeciv
VERSION = 2.5.9

# These two variables depend on the location of this directory.
subdir = translations/freeciv
top_builddir = ../..

SHELL = /bin/sh


srcdir = .
top_srcdir = ../..


prefix = /usr/local
exec_prefix = ${prefix}
datarootdir = ${prefix}/share
datadir = ${datarootdir}
localedir = ${datarootdir}/locale
gettextsrcdir = $(datadir)/gettext/po

INSTALL = /usr/bin/install -c
INSTALL_DATA = ${INSTALL} -m 644
MKINSTALLDIRS = bootstrap/mkinstalldirs
mkinstalldirs = /bin/mkdir -p

CC = gcc
GMSGFMT = /usr/bin/msgfmt
MSGFMT = /usr/bin/msgfmt
XGETTEXT = /usr/bin/xgettext
MSGMERGE = msgmerge

DEFS = -DHAVE_CONFIG_H
CFLAGS = -Wno-tautological-compare -Wno-deprecated-declarations -Wno-nonnull-compare -Wall -Wpointer-arith -Wcast-align -Werror -Wmissing-prototypes -Wmissing-declarations -Wformat -Wformat-security -Wnested-externs -g -O2 -fsigned-char
CPPFLAGS =  -DAI_MODULEDIR="\"${exec_prefix}/lib/fcai\"" -DLOCALEDIR="\"${datarootdir}/locale\"" -DBINDIR="\"${exec_prefix}/bin\"" -DFC_CONF_PATH="\"${prefix}/etc/freeciv\"" -DDEFAULT_DATA_PATH="\".:data:~/.freeciv/2.5:${datarootdir}/freeciv\"" -DDEFAULT_SAVE_PATH="\".:~/.freeciv/saves\"" -DDEFAULT_SCENARIO_PATH="\".:data/scenarios:~/.freeciv/2.5/scenarios:~/.freeciv/scenarios:${datarootdir}/freeciv/scenarios\""

INCLUDES = -I.. -I$(top_srcdir)/intl

COMPILE = $(CC) -c $(DEFS) $(INCLUDES) $(CPPFLAGS) $(CFLAGS) $(XCFLAGS)

POFILES =  ar.po bg.po cs.po ca.po da.po de.po el.po en_GB.po eo.po es.po et.po fa.po fi.po fr.po ga.po gd.po he.po hu.po id.po it.po ja.po ko.po lt.po nl.po nb.po pl.po pt.po pt_BR.po ro.po ru.po sr.po sv.po tr.po uk.po zh_CN.po zh_TW.po
GMOFILES =  ar.gmo bg.gmo cs.gmo ca.gmo da.gmo de.gmo el.gmo en_GB.gmo eo.gmo es.gmo et.gmo fa.gmo fi.gmo fr.gmo ga.gmo gd.gmo he.gmo hu.gmo id.gmo it.gmo ja.gmo ko.gmo lt.gmo nl.gmo nb.gmo pl.gmo pt.gmo pt_BR.gmo ro.gmo ru.gmo sr.gmo sv.gmo tr.gmo uk.gmo zh_CN.gmo zh_TW.gmo

# Keep this in one line, since configure replaces it when NLS disabled
DISTFILES = Makefile.in.in POTFILES.in POTFILES.skip $(PACKAGE).pot $(POFILES) $(GMOFILES)

POTFILES = \
	../../ai/default/advdiplomacy.c \
	../../ai/default/aicity.c \
	../../ai/threaded/threadedai.c \
	../../client/agents/cma_core.c \
	../../client/agents/cma_fec.c \
	../../client/attribute.c \
	../../client/audio.c \
	../../client/chatline_common.c \
	../../client/citydlg_common.c \
	../../client/cityrepdata.c \
	../../client/client_main.c \
	../../client/climisc.c \
	../../client/clinet.c \
	../../client/connectdlg_common.c \
	../../client/control.c \
	../../client/editor.c \
	../../client/ggzclient.c \
	../../client/global_worklist.c \
	../../client/gui-gtk-2.0/caravan_dialog.c \
	../../client/gui-gtk-2.0/chatline.c \
	../../client/gui-gtk-2.0/citizensinfo.c \
	../../client/gui-gtk-2.0/citydlg.c \
	../../client/gui-gtk-2.0/cityrep.c \
	../../client/gui-gtk-2.0/cma_fe.c \
	../../client/gui-gtk-2.0/dialogs.c \
	../../client/gui-gtk-2.0/diplodlg.c \
	../../client/gui-gtk-2.0/diplomat_dialog.c \
	../../client/gui-gtk-2.0/editgui.c \
	../../client/gui-gtk-2.0/editprop.c \
	../../client/gui-gtk-2.0/finddlg.c \
	../../client/gui-gtk-2.0/gamedlgs.c \
	../../client/gui-gtk-2.0/gotodlg.c \
	../../client/gui-gtk-2.0/gui_main.c \
	../../client/gui-gtk-2.0/gui_stuff.c \
	../../client/gui-gtk-2.0/happiness.c \
	../../client/gui-gtk-2.0/helpdlg.c \
	../../client/gui-gtk-2.0/inteldlg.c \
	../../client/gui-gtk-2.0/luaconsole.c \
	../../client/gui-gtk-2.0/mapctrl.c \
	../../client/gui-gtk-2.0/mapview.c \
	../../client/gui-gtk-2.0/menu.c \
	../../client/gui-gtk-2.0/messagedlg.c \
	../../client/gui-gtk-2.0/messagewin.c \
	../../client/gui-gtk-2.0/optiondlg.c \
	../../client/gui-gtk-2.0/pages.c \
	../../client/gui-gtk-2.0/plrdlg.c \
	../../client/gui-gtk-2.0/repodlgs.c \
	../../client/gui-gtk-2.0/soundset_dlg.c \
	../../client/gui-gtk-2.0/spaceshipdlg.c \
	../../client/gui-gtk-2.0/theme_dlg.c \
	../../client/gui-gtk-2.0/tileset_dlg.c \
	../../client/gui-gtk-2.0/unitselect.c \
	../../client/gui-gtk-2.0/voteinfo_bar.c \
	../../client/gui-gtk-2.0/wldlg.c \
	../../client/gui-gtk-3.0/caravan_dialog.c \
	../../client/gui-gtk-3.0/chatline.c \
	../../client/gui-gtk-3.0/citizensinfo.c \
	../../client/gui-gtk-3.0/citydlg.c \
	../../client/gui-gtk-3.0/cityrep.c \
	../../client/gui-gtk-3.0/cma_fe.c \
	../../client/gui-gtk-3.0/dialogs.c \
	../../client/gui-gtk-3.0/diplodlg.c \
	../../client/gui-gtk-3.0/diplomat_dialog.c \
	../../client/gui-gtk-3.0/editgui.c \
	../../client/gui-gtk-3.0/editprop.c \
	../../client/gui-gtk-3.0/finddlg.c \
	../../client/gui-gtk-3.0/gamedlgs.c \
	../../client/gui-gtk-3.0/gotodlg.c \
	../../client/gui-gtk-3.0/gui_main.c \
	../../client/gui-gtk-3.0/gui_stuff.c \
	../../client/gui-gtk-3.0/happiness.c \
	../../client/gui-gtk-3.0/helpdlg.c \
	../../client/gui-gtk-3.0/inteldlg.c \
	../../client/gui-gtk-3.0/luaconsole.c \
	../../client/gui-gtk-3.0/mapctrl.c \
	../../client/gui-gtk-3.0/mapview.c \
	../../client/gui-gtk-3.0/menu.c \
	../../client/gui-gtk-3.0/messagedlg.c \
	../../client/gui-gtk-3.0/messagewin.c \
	../../client/gui-gtk-3.0/optiondlg.c \
	../../client/gui-gtk-3.0/pages.c \
	../../client/gui-gtk-3.0/plrdlg.c \
	../../client/gui-gtk-3.0/repodlgs.c \
	../../client/gui-gtk-3.0/soundset_dlg.c \
	../../client/gui-gtk-3.0/spaceshipdlg.c \
	../../client/gui-gtk-3.0/theme_dlg.c \
	../../client/gui-gtk-3.0/tileset_dlg.c \
	../../client/gui-gtk-3.0/unitselect.c \
	../../client/gui-gtk-3.0/voteinfo_bar.c \
	../../client/gui-gtk-3.0/wldlg.c \
	../../client/gui-qt/chatline.cpp \
	../../client/gui-qt/citydlg.cpp \
	../../client/gui-qt/cityrep.cpp \
	../../client/gui-qt/dialogs.cpp \
	../../client/gui-qt/diplodlg.cpp \
	../../client/gui-qt/fc_client.cpp \
	../../client/gui-qt/gui_main.cpp \
	../../client/gui-qt/gotodlg.cpp \
	../../client/gui-qt/helpdlg.cpp \
	../../client/gui-qt/mapctrl.cpp \
	../../client/gui-qt/mapview.cpp \
	../../client/gui-qt/menu.cpp \
	../../client/gui-qt/messagedlg.cpp \
	../../client/gui-qt/messagewin.cpp \
	../../client/gui-qt/optiondlg.cpp \
	../../client/gui-qt/pages.cpp \
	../../client/gui-qt/plrdlg.cpp \
	../../client/gui-qt/ratesdlg.cpp \
	../../client/gui-qt/repodlgs.cpp \
	../../client/gui-qt/spaceshipdlg.cpp \
	../../client/gui-qt/voteinfo_bar.cpp \
	../../client/gui-sdl/caravan_dialog.c \
	../../client/gui-sdl/chatline.c \
	../../client/gui-sdl/citydlg.c \
	../../client/gui-sdl/cityrep.c \
	../../client/gui-sdl/cma_fe.c \
	../../client/gui-sdl/connectdlg.c \
	../../client/gui-sdl/dialogs.c \
	../../client/gui-sdl/diplodlg.c \
	../../client/gui-sdl/diplomat_dialog.c \
	../../client/gui-sdl/finddlg.c \
	../../client/gui-sdl/gotodlg.c \
	../../client/gui-sdl/graphics.c \
	../../client/gui-sdl/gui_main.c \
	../../client/gui-sdl/gui_tilespec.c \
	../../client/gui-sdl/helpdlg.c \
	../../client/gui-sdl/inteldlg.c \
	../../client/gui-sdl/mapctrl.c \
	../../client/gui-sdl/mapview.c \
	../../client/gui-sdl/menu.c \
	../../client/gui-sdl/messagewin.c \
	../../client/gui-sdl/optiondlg.c \
	../../client/gui-sdl/pages.c \
	../../client/gui-sdl/plrdlg.c \
	../../client/gui-sdl/repodlgs.c \
	../../client/gui-sdl/spaceshipdlg.c \
	../../client/gui-sdl/sprite.c \
	../../client/gui-sdl/themes.c \
	../../client/gui-sdl/themespec.c \
	../../client/gui-sdl/wldlg.c \
	../../client/gui-xaw/chatline.c \
	../../client/gui-xaw/citydlg.c \
	../../client/gui-xaw/cityrep.c \
	../../client/gui-xaw/cma_fe.c \
	../../client/gui-xaw/connectdlg.c \
	../../client/gui-xaw/dialogs.c \
	../../client/gui-xaw/diplodlg.c \
	../../client/gui-xaw/diplomat_dialog.c \
	../../client/gui-xaw/gui_main.c \
	../../client/gui-xaw/helpdlg.c \
	../../client/gui-xaw/inputdlg.c \
	../../client/gui-xaw/inteldlg.c \
	../../client/gui-xaw/mapctrl.c \
	../../client/gui-xaw/menu.c \
	../../client/gui-xaw/messagedlg.c \
	../../client/gui-xaw/optiondlg.c \
	../../client/gui-xaw/pages.c \
	../../client/gui-xaw/plrdlg.c \
	../../client/gui-xaw/ratesdlg.c \
	../../client/gui-xaw/repodlgs.c \
	../../client/gui-xaw/wldlg.c \
	../../client/helpdata.c \
	../../client/include/helpdlg_g.h \
	../../client/luaconsole_common.c \
	../../client/mapctrl_common.c \
	../../client/options.c \
	../../client/packhand.c \
	../../client/plrdlg_common.c \
	../../client/repodlgs_common.c \
	../../client/servers.c \
	../../client/text.c \
	../../client/tilespec.c \
	../../common/ai.c \
	../../common/city.c \
	../../common/connection.c \
	../../common/effects.c \
	../../common/events.c \
	../../common/fc_cmdhelp.c \
	../../common/game.c \
	../../common/game.h \
	../../common/government.c \
	../../common/mapimg.c \
	../../common/packets.c \
	../../common/player.c \
	../../common/requirements.c \
	../../common/team.c \
	../../common/tech.c \
	../../common/tech.h \
	../../common/terrain.c \
	../../common/terrain.h \
	../../common/unit.c \
	../../common/unittype.c \
	../../common/unittype.h \
	../../common/version.c \
	../../data/civ1/buildings.ruleset \
	../../data/civ1/cities.ruleset \
	../../data/civ1/game.ruleset \
	../../data/civ1/governments.ruleset \
	../../data/civ1/nations.ruleset \
	../../data/civ1/script.lua \
	../../data/civ1/techs.ruleset \
	../../data/civ1/terrain.ruleset \
	../../data/civ1/units.ruleset \
	../../data/civ2/buildings.ruleset \
	../../data/civ2/cities.ruleset \
	../../data/civ2/game.ruleset \
	../../data/civ2/governments.ruleset \
	../../data/civ2/nations.ruleset \
	../../data/civ2/script.lua \
	../../data/civ2/techs.ruleset \
	../../data/civ2/terrain.ruleset \
	../../data/civ2/units.ruleset \
	../../data/default/default.lua \
	../../data/default/nationlist.ruleset \
	../../data/classic/buildings.ruleset \
	../../data/classic/cities.ruleset \
	../../data/classic/game.ruleset \
	../../data/classic/governments.ruleset \
	../../data/classic/script.lua \
	../../data/classic/techs.ruleset \
	../../data/classic/terrain.ruleset \
	../../data/classic/units.ruleset \
	../../data/experimental/buildings.ruleset \
	../../data/experimental/cities.ruleset \
	../../data/experimental/game.ruleset \
	../../data/experimental/governments.ruleset \
	../../data/experimental/script.lua \
	../../data/experimental/techs.ruleset \
	../../data/experimental/terrain.ruleset \
	../../data/experimental/units.ruleset \
	../../data/civ2civ3/buildings.ruleset \
	../../data/civ2civ3/cities.ruleset \
	../../data/civ2civ3/game.ruleset \
	../../data/civ2civ3/governments.ruleset \
	../../data/civ2civ3/script.lua \
	../../data/civ2civ3/techs.ruleset \
	../../data/civ2civ3/terrain.ruleset \
	../../data/civ2civ3/units.ruleset \
	../../data/Freeciv.in \
	../../data/helpdata.txt \
	../../data/multiplayer/buildings.ruleset \
	../../data/multiplayer/cities.ruleset \
	../../data/multiplayer/game.ruleset \
	../../data/multiplayer/governments.ruleset \
	../../data/multiplayer/script.lua \
	../../data/multiplayer/techs.ruleset \
	../../data/multiplayer/terrain.ruleset \
	../../data/multiplayer/units.ruleset \
	../../data/multiplayer/script.lua \
	../../data/nation/american.ruleset \
	../../data/nation/apache.ruleset \
	../../data/nation/arab.ruleset \
	../../data/nation/austrian.ruleset \
	../../data/nation/aztec.ruleset \
	../../data/nation/babylonian.ruleset \
	../../data/nation/barbarian.ruleset \
	../../data/nation/brazilian.ruleset \
	../../data/nation/byzantium.ruleset \
	../../data/nation/carthaginian.ruleset \
	../../data/nation/celtic.ruleset \
	../../data/nation/cherokee.ruleset \
	../../data/nation/chinese.ruleset \
	../../data/nation/danish.ruleset \
	../../data/nation/dutch.ruleset \
	../../data/nation/egyptian.ruleset \
	../../data/nation/english.ruleset \
	../../data/nation/ethiopian.ruleset \
	../../data/nation/french.ruleset \
	../../data/nation/german.ruleset \
	../../data/nation/greek.ruleset \
	../../data/nation/hittite.ruleset \
	../../data/nation/hunnic.ruleset \
	../../data/nation/inca.ruleset \
	../../data/nation/indian.ruleset \
	../../data/nation/indonesian.ruleset \
	../../data/nation/iroquois.ruleset \
	../../data/nation/italian.ruleset \
	../../data/nation/japanese.ruleset \
	../../data/nation/khmer.ruleset \
	../../data/nation/korean.ruleset \
	../../data/nation/mali.ruleset \
	../../data/nation/mayan.ruleset \
	../../data/nation/mongol.ruleset \
	../../data/nation/persian.ruleset \
	../../data/nation/pirate.ruleset \
	../../data/nation/polish.ruleset \
	../../data/nation/polynesian.ruleset \
	../../data/nation/portuguese.ruleset \
	../../data/nation/roman.ruleset \
	../../data/nation/russian.ruleset \
	../../data/nation/sioux.ruleset \
	../../data/nation/songhai.ruleset \
	../../data/nation/spanish.ruleset \
	../../data/nation/sumerian.ruleset \
	../../data/nation/swedish.ruleset \
	../../data/nation/taino.ruleset \
	../../data/nation/thai.ruleset \
	../../data/nation/tupi.ruleset \
	../../data/nation/turk.ruleset \
	../../data/nation/viking.ruleset \
	../../data/nation/zulu.ruleset \
	../../data/scenarios/british-isles-85x80-v2.80.sav \
	../../data/scenarios/earth-160x90-v2.sav \
	../../data/scenarios/earth-80x50-v3.sav \
	../../data/scenarios/europe-200x100-v2.sav \
	../../data/scenarios/france-140x90-v2.sav \
	../../data/scenarios/hagworld-120x60-v1.2.sav \
	../../data/scenarios/iberian-peninsula-136x100-v1.0.sav \
	../../data/scenarios/italy-100x100-v1.5.sav \
	../../data/scenarios/japan-88x100-v1.3.sav \
	../../data/scenarios/north_america_116x100-v1.2.sav \
	../../data/scenarios/tutorial.sav \
	../../translations/Strings.txt \
	../../tools/civmanual.c \
	../../tools/download.c \
	../../tools/modinst.c \
	../../tools/modinst.h \
	../../tools/mpcli.c \
	../../tools/mpcmdline.c \
	../../tools/mpgui_gtk2.c \
	../../tools/mpgui_gtk3.c \
	../../tools/mpgui_qt.cpp \
	../../tools/mpgui_qt_worker.cpp \
	../../server/aiiface.c \
	../../server/auth.c \
	../../server/barbarian.c \
	../../server/cityhand.c \
	../../server/citytools.c \
	../../server/cityturn.c \
	../../server/civserver.c \
	../../server/commands.c \
	../../server/connecthand.c \
	../../server/console.c \
	../../server/diplhand.c \
	../../server/diplomats.c \
	../../server/edithand.c \
	../../server/fcdb.c \
	../../server/gamehand.c \
	../../server/generator/mapgen.c \
	../../server/generator/mapgen_topology.c \
	../../server/generator/startpos.c \
	../../server/ggzserver.c \
	../../server/handchat.c \
	../../server/maphand.c \
	../../server/meta.c \
	../../server/plrhand.c \
	../../server/report.c \
	../../server/ruleset.c \
	../../server/savegame2.c \
	../../server/savegame.c \
	../../server/sernet.c \
	../../server/settings.c \
	../../server/settings.h \
	../../server/spacerace.c \
	../../server/srv_main.c \
	../../server/stdinhand.c \
	../../server/techtools.c \
	../../server/unithand.c \
	../../server/unittools.c \
	../../server/voting.c \
	../../utility/astring.c \
	../../utility/fciconv.c \
	../../utility/inputfile.c \
	../../utility/log.c \
	../../utility/netfile.c \
	../../utility/netintf.c \
	../../utility/registry_ini.c \
	../../utility/shared.c \
	../../utility/support.c

CATALOGS =  ar.gmo bg.gmo cs.gmo ca.gmo da.gmo de.gmo el.gmo en_GB.gmo eo.gmo es.gmo et.gmo fa.gmo fi.gmo fr.gmo ga.gmo gd.gmo he.gmo hu.gmo id.gmo it.gmo ja.gmo ko.gmo lt.gmo nl.gmo nb.gmo pl.gmo pt.gmo pt_BR.gmo ro.gmo ru.gmo sr.gmo sv.gmo tr.gmo uk.gmo zh_CN.gmo zh_TW.gmo

.SUFFIXES:
.SUFFIXES: .c .o .po .pox .gmo .mo

.c.o:
	$(COMPILE) $<

.po.pox:
	$(MAKE) $(PACKAGE).pot
	$(MSGMERGE) $< $(srcdir)/$(PACKAGE).pot -o $*.pox

.po.mo:
	$(MSGFMT) -o $@ $<

.po.gmo:
	file=$(srcdir)/`echo $* | sed 's,.*/,,'`.gmo \
	  && rm -f $$file && $(GMSGFMT) -c --statistics -o $$file $<


all: all-yes

all-yes: $(CATALOGS)
all-no:

# Note: Target 'all' must not depend on target '$(srcdir)/$(PACKAGE).pot',
# otherwise packages like GCC can not be built if only parts of the source
# have been downloaded.

$(srcdir)/$(PACKAGE).pot: $(POTFILES) $(srcdir)/POTFILES.in
	$(XGETTEXT) --default-domain=$(PACKAGE) --directory=$(top_srcdir) \
	  --add-comments="TRANS:" --escape --keyword=_ --keyword=N_ --keyword=Q_ --keyword=PL_:1,2 \
	  --language=C --files-from=$(srcdir)/POTFILES.in \
	&& test ! -f $(PACKAGE).po \
	   || ( rm -f $(srcdir)/$(PACKAGE).pot \
		&& mv $(PACKAGE).po $(srcdir)/$(PACKAGE).pot )


install: install-exec install-data
install-exec:
install-data: install-data-yes
	if test "$(PACKAGE)" = "gettext"; then \
	  $(mkinstalldirs) $(DESTDIR)$(gettextsrcdir); \
	  $(INSTALL_DATA) $(srcdir)/Makefile.in.in \
			  $(DESTDIR)$(gettextsrcdir)/Makefile.in.in; \
	else \
	  : ; \
	fi
install-data-no: all
install-data-yes: all
	$(mkinstalldirs) $(DESTDIR)$(datadir)
	@catalogs='$(CATALOGS)'; \
	for cat in $$catalogs; do \
	  cat=`basename $$cat`; \
	  lang=`echo $$cat | sed 's/\.gmo$$//'`; \
	  dir=$(localedir)/$$lang/LC_MESSAGES; \
	  $(mkinstalldirs) $(DESTDIR)$$dir; \
	  if test -r $$cat; then \
	    $(INSTALL_DATA) $$cat $(DESTDIR)$$dir/$(PACKAGE).mo; \
	    echo "installing $$cat as $(DESTDIR)$$dir/$(PACKAGE).mo"; \
	  else \
	    $(INSTALL_DATA) $(srcdir)/$$cat $(DESTDIR)$$dir/$(PACKAGE).mo; \
	    echo "installing $(srcdir)/$$cat as" \
		 "$(DESTDIR)$$dir/$(PACKAGE).mo"; \
	  fi; \
	done

# Define this as empty until I found a useful application.
installcheck:

uninstall:
	catalogs='$(CATALOGS)'; \
	for cat in $$catalogs; do \
	  cat=`basename $$cat`; \
	  lang=`echo $$cat | sed 's/\.gmo$$//'`; \
	  rm -f $(DESTDIR)$(localedir)/$$lang/LC_MESSAGES/$(PACKAGE).mo; \
	done
	if test "$(PACKAGE)" = "gettext"; then \
	  rm -f $(DESTDIR)$(gettextsrcdir)/Makefile.in.in; \
	else \
	  : ; \
	fi

check: all

dvi info tags TAGS ID:

mostlyclean:
	rm -f core core.* *.pox $(PACKAGE).po *.new.po
	rm -fr *.o

clean: mostlyclean

distclean: clean
	rm -f Makefile Makefile.in POTFILES *.mo

maintainer-clean: distclean
	@echo "This command is intended for maintainers to use;"
	@echo "it deletes files that may require special tools to rebuild."
	rm -f $(GMOFILES)

distdir = $(top_builddir)/$(PACKAGE)-$(VERSION)/$(subdir)

# If you change following line, update also configure.ac where it
# builds Makefile if NLS disabled.
dist distdir:
	$(MAKE) update-po
	@$(MAKE) dist2
# This is a separate target because 'update-po' must be executed before.
dist2: $(DISTFILES)
	dists="$(DISTFILES)"; \
	for file in $$dists; do \
	  if test -f $$file; then dir=.; else dir=$(srcdir); fi; \
	  cp -p $$dir/$$file $(distdir); \
	done

# If you change following line, update also configure.ac where it
# builds Makefile if NLS disabled.
update-po: Makefile
	$(MAKE) $(PACKAGE).pot
	if test "$(PACKAGE)" = "gettext"; then PATH=`pwd`/../src:$$PATH; fi; \
	cd $(srcdir); \
	catalogs='$(GMOFILES)'; \
	for cat in $$catalogs; do \
	  cat=`basename $$cat`; \
	  lang=`echo $$cat | sed 's/\.gmo$$//'`; \
	  if test -x "$(builddir)/$$lang.po" ; then \
	    dir="$(builddir)"; \
	  else \
	    dir=. ; \
	  fi; \
	  echo "$$lang:"; \
	  if $(MSGMERGE) $$dir/$$lang.po $(PACKAGE).pot -o $$lang.new.po; then \
	    mv -f $$lang.new.po $$dir/$$lang.po; \
	  else \
	    echo "msgmerge for $$cat failed!"; \
	    rm -f $$lang.new.po; \
	  fi; \
	done
	$(MAKE) update-gmo

update-gmo: Makefile $(GMOFILES)
	@:

Makefile: Makefile.in.in $(top_builddir)/config.status POTFILES.in
	cd $(top_builddir) \
	  && CONFIG_FILES=$(subdir)/$@.in CONFIG_HEADERS= \
	       $(SHELL) ./config.status

# Tell versions [3.59,3.63) of GNU make not to export all variables.
# Otherwise a system limit (for SysV at least) may be exceeded.
.NOEXPORT:
