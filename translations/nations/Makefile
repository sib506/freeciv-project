# Makefile for program source directory in GNU NLS utilities package.
# Copyright (C) 1995-1997, 2000, 2001 by Ulrich Drepper <drepper@gnu.ai.mit.edu>
#
# This file file be copied and used freely without restrictions.  It can
# be used in projects which are not available under the GNU Public License
# but which still want to provide support for the GNU gettext functionality.
# Please note that the actual code is *not* freely available.

PACKAGE = freeciv-nations
VERSION = 2.5.9

# These two variables depend on the location of this directory.
subdir = translations/nations
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
CFLAGS = -Wno-tautological-compare -Wno-deprecated-declarations -Wno-nonnull-compare -Wall -Wpointer-arith -Wcast-align -g -O0 -fsigned-char
CPPFLAGS =  -DLOCALEDIR="\"${datarootdir}/locale\"" -DBINDIR="\"${exec_prefix}/bin\"" -DFC_CONF_PATH="\"${prefix}/etc/freeciv\"" -DDEFAULT_DATA_PATH="\".:data:~/.freeciv/2.5:${datarootdir}/freeciv\"" -DDEFAULT_SAVE_PATH="\".:~/.freeciv/saves\"" -DDEFAULT_SCENARIO_PATH="\".:data/scenarios:~/.freeciv/2.5/scenarios:~/.freeciv/scenarios:${datarootdir}/freeciv/scenarios\""

INCLUDES = -I.. -I$(top_srcdir)/intl

COMPILE = $(CC) -c $(DEFS) $(INCLUDES) $(CPPFLAGS) $(CFLAGS) $(XCFLAGS)

POFILES =  ar.po bg.po cs.po ca.po da.po de.po el.po en_GB.po eo.po es.po et.po fa.po fi.po fr.po ga.po gd.po he.po hu.po id.po it.po ja.po ko.po lt.po nl.po nb.po pl.po pt.po pt_BR.po ro.po ru.po sr.po sv.po tr.po uk.po zh_CN.po zh_TW.po
GMOFILES =  ar.gmo bg.gmo cs.gmo ca.gmo da.gmo de.gmo el.gmo en_GB.gmo eo.gmo es.gmo et.gmo fa.gmo fi.gmo fr.gmo ga.gmo gd.gmo he.gmo hu.gmo id.gmo it.gmo ja.gmo ko.gmo lt.gmo nl.gmo nb.gmo pl.gmo pt.gmo pt_BR.gmo ro.gmo ru.gmo sr.gmo sv.gmo tr.gmo uk.gmo zh_CN.gmo zh_TW.gmo

# Keep this in one line, since configure replaces it when NLS disabled
DISTFILES = Makefile.in.in POTFILES.in POTFILES.skip $(PACKAGE).pot $(POFILES) $(GMOFILES)

POTFILES = \
	../../data/nation/abkhaz.ruleset \
	../../data/nation/aborigines.ruleset \
	../../data/nation/abyssinian.ruleset \
	../../data/nation/acadian.ruleset \
	../../data/nation/acehnese.ruleset \
	../../data/nation/acrean.ruleset \
	../../data/nation/afghani.ruleset \
	../../data/nation/african.ruleset \
	../../data/nation/ainu.ruleset \
	../../data/nation/akwe.ruleset \
	../../data/nation/alandalus.ruleset \
	../../data/nation/alander.ruleset \
	../../data/nation/albanian.ruleset \
	../../data/nation/aleut.ruleset \
	../../data/nation/algerian.ruleset \
	../../data/nation/alsatian.ruleset \
	../../data/nation/amazigh.ruleset \
	../../data/nation/amazonian.ruleset \
	../../data/nation/andorran.ruleset \
	../../data/nation/angolan.ruleset \
	../../data/nation/anhaltian.ruleset \
	../../data/nation/anishinaabe.ruleset \
	../../data/nation/antarctican.ruleset \
	../../data/nation/antiguan.ruleset \
	../../data/nation/antillean.ruleset \
	../../data/nation/aragonese.ruleset \
	../../data/nation/aramean.ruleset \
	../../data/nation/argentine.ruleset \
	../../data/nation/armenian.ruleset \
	../../data/nation/ashanti.ruleset \
	../../data/nation/assamese.ruleset \
	../../data/nation/assyrian.ruleset \
	../../data/nation/asturian.ruleset \
	../../data/nation/atlantean.ruleset \
	../../data/nation/australian.ruleset \
	../../data/nation/avar.ruleset \
	../../data/nation/aymara.ruleset \
	../../data/nation/azeri.ruleset \
	../../data/nation/badian.ruleset \
	../../data/nation/bahamian.ruleset \
	../../data/nation/bahraini.ruleset \
	../../data/nation/bangladeshi.ruleset \
	../../data/nation/barbadian.ruleset \
	../../data/nation/bashkir.ruleset \
	../../data/nation/basque.ruleset \
	../../data/nation/bavarian.ruleset \
	../../data/nation/belarusian.ruleset \
	../../data/nation/belgian.ruleset \
	../../data/nation/belgic.ruleset \
	../../data/nation/belizean.ruleset \
	../../data/nation/bengali.ruleset \
	../../data/nation/beninese.ruleset \
	../../data/nation/benin.ruleset \
	../../data/nation/bhutanese.ruleset \
	../../data/nation/biafran.ruleset \
	../../data/nation/bissauguinean.ruleset \
	../../data/nation/boer.ruleset \
	../../data/nation/boian.ruleset \
	../../data/nation/bolivian.ruleset \
	../../data/nation/bosnia.ruleset \
	../../data/nation/bosporan.ruleset \
	../../data/nation/botswanan.ruleset \
	../../data/nation/brandenburgian.ruleset \
	../../data/nation/breton.ruleset \
	../../data/nation/british.ruleset \
	../../data/nation/briton.ruleset \
	../../data/nation/bruneian.ruleset \
	../../data/nation/bulgarian.ruleset \
	../../data/nation/burgundian.ruleset \
	../../data/nation/burgundic.ruleset \
	../../data/nation/burkinabe.ruleset \
	../../data/nation/burmese.ruleset \
	../../data/nation/burundi.ruleset \
	../../data/nation/buryat.ruleset \
	../../data/nation/californian.ruleset \
	../../data/nation/cambodian.ruleset \
	../../data/nation/cameroonian.ruleset \
	../../data/nation/canadian.ruleset \
	../../data/nation/canari.ruleset \
	../../data/nation/cantonese.ruleset \
	../../data/nation/capeverdean.ruleset \
	../../data/nation/carantanian.ruleset \
	../../data/nation/castilian.ruleset \
	../../data/nation/catalan.ruleset \
	../../data/nation/celtiberian.ruleset \
	../../data/nation/centralafrican.ruleset \
	../../data/nation/centralamerican.ruleset \
	../../data/nation/centrallithuanian.ruleset \
	../../data/nation/chadian.ruleset \
	../../data/nation/cham.ruleset \
	../../data/nation/chananean.ruleset \
	../../data/nation/chechen.ruleset \
	../../data/nation/chiapanec.ruleset \
	../../data/nation/chickasaw.ruleset \
	../../data/nation/chilean.ruleset \
	../../data/nation/chimu.ruleset \
	../../data/nation/chinook.ruleset \
	../../data/nation/choctaw.ruleset \
	../../data/nation/chola.ruleset \
	../../data/nation/chrobatian.ruleset \
	../../data/nation/chumash.ruleset \
	../../data/nation/chuvash.ruleset \
	../../data/nation/circassian.ruleset \
	../../data/nation/colombian.ruleset \
	../../data/nation/comanche.ruleset \
	../../data/nation/comorian.ruleset \
	../../data/nation/confederate.ruleset \
	../../data/nation/congolesebrazzaville.ruleset \
	../../data/nation/congolese.ruleset \
	../../data/nation/cornish.ruleset \
	../../data/nation/corsican.ruleset \
	../../data/nation/cossack.ruleset \
	../../data/nation/costarican.ruleset \
	../../data/nation/cree.ruleset \
	../../data/nation/cretan.ruleset \
	../../data/nation/crimeantatar.ruleset \
	../../data/nation/croatian.ruleset \
	../../data/nation/crusader.ruleset \
	../../data/nation/cuban.ruleset \
	../../data/nation/curonian.ruleset \
	../../data/nation/cuyavian.ruleset \
	../../data/nation/cypriot.ruleset \
	../../data/nation/cyrenaican.ruleset \
	../../data/nation/czechoslovak.ruleset \
	../../data/nation/czech.ruleset \
	../../data/nation/dacian.ruleset \
	../../data/nation/dahomean.ruleset \
	../../data/nation/darfuri.ruleset \
	../../data/nation/djiboutian.ruleset \
	../../data/nation/dominicano.ruleset \
	../../data/nation/dominican.ruleset \
	../../data/nation/dryad.ruleset \
	../../data/nation/eastgerman.ruleset \
	../../data/nation/easttimorese.ruleset \
	../../data/nation/ecuadorian.ruleset \
	../../data/nation/egyptianarab.ruleset \
	../../data/nation/elamite.ruleset \
	../../data/nation/emirati.ruleset \
	../../data/nation/epirote.ruleset \
	../../data/nation/equatoguinean.ruleset \
	../../data/nation/eritrean.ruleset \
	../../data/nation/esperant.ruleset \
	../../data/nation/estonian.ruleset \
	../../data/nation/etruscan.ruleset \
	../../data/nation/european.ruleset \
	../../data/nation/evenki.ruleset \
	../../data/nation/faroese.ruleset \
	../../data/nation/fijian.ruleset \
	../../data/nation/filipino.ruleset \
	../../data/nation/finnish.ruleset \
	../../data/nation/flemish.ruleset \
	../../data/nation/florentine.ruleset \
	../../data/nation/formosan.ruleset \
	../../data/nation/franconian.ruleset \
	../../data/nation/frankish.ruleset \
	../../data/nation/frisian.ruleset \
	../../data/nation/friulian.ruleset \
	../../data/nation/fulani.ruleset \
	../../data/nation/gabonese.ruleset \
	../../data/nation/gaelic.ruleset \
	../../data/nation/galician.ruleset \
	../../data/nation/gallic.ruleset \
	../../data/nation/gambian.ruleset \
	../../data/nation/genoese.ruleset \
	../../data/nation/georgian.ruleset \
	../../data/nation/gepid.ruleset \
	../../data/nation/germanbelgian.ruleset \
	../../data/nation/germanic.ruleset \
	../../data/nation/ghanaian.ruleset \
	../../data/nation/ghana.ruleset \
	../../data/nation/ghaznavid.ruleset \
	../../data/nation/gokturk.ruleset \
	../../data/nation/goldenhorde.ruleset \
	../../data/nation/gothic.ruleset \
	../../data/nation/greaterpolish.ruleset \
	../../data/nation/greenlander.ruleset \
	../../data/nation/grenadian.ruleset \
	../../data/nation/guanche.ruleset \
	../../data/nation/guarani.ruleset \
	../../data/nation/guatemalan.ruleset \
	../../data/nation/guinean.ruleset \
	../../data/nation/gupta.ruleset \
	../../data/nation/guyanese.ruleset \
	../../data/nation/hacker.ruleset \
	../../data/nation/haitian.ruleset \
	../../data/nation/hanoverian.ruleset \
	../../data/nation/han.ruleset \
	../../data/nation/hansa.ruleset \
	../../data/nation/hasinay.ruleset \
	../../data/nation/hawaiian.ruleset \
	../../data/nation/hellenic.ruleset \
	../../data/nation/helvetian.ruleset \
	../../data/nation/hephthalite.ruleset \
	../../data/nation/hessian.ruleset \
	../../data/nation/himyarite.ruleset \
	../../data/nation/holyroman.ruleset \
	../../data/nation/holysee.ruleset \
	../../data/nation/honduran.ruleset \
	../../data/nation/hopi.ruleset \
	../../data/nation/hungarian.ruleset \
	../../data/nation/iberian.ruleset \
	../../data/nation/icelandic.ruleset \
	../../data/nation/illyrian.ruleset \
	../../data/nation/indoeuropean.ruleset \
	../../data/nation/inuit.ruleset \
	../../data/nation/iranian.ruleset \
	../../data/nation/iraqi.ruleset \
	../../data/nation/irish.ruleset \
	../../data/nation/israeli.ruleset \
	../../data/nation/israelite.ruleset \
	../../data/nation/italiangreek.ruleset \
	../../data/nation/ivoirian.ruleset \
	../../data/nation/jaffna.ruleset \
	../../data/nation/jamaican.ruleset \
	../../data/nation/jolof.ruleset \
	../../data/nation/jordanian.ruleset \
	../../data/nation/kalmyk.ruleset \
	../../data/nation/kanem-bornu.ruleset \
	../../data/nation/karabakhi.ruleset \
	../../data/nation/karelian.ruleset \
	../../data/nation/karen.ruleset \
	../../data/nation/kashmiri.ruleset \
	../../data/nation/kashubian.ruleset \
	../../data/nation/katangan.ruleset \
	../../data/nation/kazakh.ruleset \
	../../data/nation/kenyan.ruleset \
	../../data/nation/khazar.ruleset \
	../../data/nation/khoisan.ruleset \
	../../data/nation/khwarezmian.ruleset \
	../../data/nation/kiribati.ruleset \
	../../data/nation/kittitian.ruleset \
	../../data/nation/komi.ruleset \
	../../data/nation/kongo.ruleset \
	../../data/nation/kosovar.ruleset \
	../../data/nation/kuna.ruleset \
	../../data/nation/kurd.ruleset \
	../../data/nation/kushan.ruleset \
	../../data/nation/kuwaiti.ruleset \
	../../data/nation/kyrgyz.ruleset \
	../../data/nation/langobardic.ruleset \
	../../data/nation/lankese.ruleset \
	../../data/nation/laotian.ruleset \
	../../data/nation/latin.ruleset \
	../../data/nation/latvian.ruleset \
	../../data/nation/lebanese.ruleset \
	../../data/nation/lendian.ruleset \
	../../data/nation/leonese.ruleset \
	../../data/nation/lesothoan.ruleset \
	../../data/nation/liberian.ruleset \
	../../data/nation/liburnian.ruleset \
	../../data/nation/libyan.ruleset \
	../../data/nation/liechtensteiner.ruleset \
	../../data/nation/ligurian.ruleset \
	../../data/nation/lipkatatar.ruleset \
	../../data/nation/lippe.ruleset \
	../../data/nation/lithuanian.ruleset \
	../../data/nation/lojbanistani.ruleset \
	../../data/nation/lorrain.ruleset \
	../../data/nation/luwian.ruleset \
	../../data/nation/luxembourgish.ruleset \
	../../data/nation/lycian.ruleset \
	../../data/nation/maasai.ruleset \
	../../data/nation/macedonian.ruleset \
	../../data/nation/macedon.ruleset \
	../../data/nation/majapahit.ruleset \
	../../data/nation/malagasy.ruleset \
	../../data/nation/malawian.ruleset \
	../../data/nation/malaysian.ruleset \
	../../data/nation/maldivian.ruleset \
	../../data/nation/malian.ruleset \
	../../data/nation/maltese.ruleset \
	../../data/nation/mamluk.ruleset \
	../../data/nation/manchu.ruleset \
	../../data/nation/manx.ruleset \
	../../data/nation/maori.ruleset \
	../../data/nation/mapuche.ruleset \
	../../data/nation/marathi.ruleset \
	../../data/nation/marshallese.ruleset \
	../../data/nation/martian.ruleset \
	../../data/nation/mauritanian.ruleset \
	../../data/nation/mauritian.ruleset \
	../../data/nation/mazovian.ruleset \
	../../data/nation/mecklenburgian.ruleset \
	../../data/nation/median.ruleset \
	../../data/nation/messapian.ruleset \
	../../data/nation/metis.ruleset \
	../../data/nation/mexican.ruleset \
	../../data/nation/miao.ruleset \
	../../data/nation/micronesian.ruleset \
	../../data/nation/mikmaq.ruleset \
	../../data/nation/milanese.ruleset \
	../../data/nation/miskito.ruleset \
	../../data/nation/mitanni.ruleset \
	../../data/nation/mixtec.ruleset \
	../../data/nation/moldovan.ruleset \
	../../data/nation/moluccan.ruleset \
	../../data/nation/monegasque.ruleset \
	../../data/nation/mon.ruleset \
	../../data/nation/montenegrin.ruleset \
	../../data/nation/moravian.ruleset \
	../../data/nation/mordvin.ruleset \
	../../data/nation/moroccan.ruleset \
	../../data/nation/mozambican.ruleset \
	../../data/nation/mughal.ruleset \
	../../data/nation/muscovite.ruleset \
	../../data/nation/muskogee.ruleset \
	../../data/nation/mwiska.ruleset \
	../../data/nation/namibian.ruleset \
	../../data/nation/nauruan.ruleset \
	../../data/nation/navajo.ruleset \
	../../data/nation/neapolitan.ruleset \
	../../data/nation/nenets.ruleset \
	../../data/nation/nepali.ruleset \
	../../data/nation/nestorian.ruleset \
	../../data/nation/newfoundland.ruleset \
	../../data/nation/newzealand.ruleset \
	../../data/nation/nicaraguan.ruleset \
	../../data/nation/nigerian.ruleset \
	../../data/nation/nigerien.ruleset \
	../../data/nation/nimiipuu.ruleset \
	../../data/nation/norman.ruleset \
	../../data/nation/northkorean.ruleset \
	../../data/nation/northumbrian.ruleset \
	../../data/nation/norwegian.ruleset \
	../../data/nation/novgorodian.ruleset \
	../../data/nation/nubian.ruleset \
	../../data/nation/numidian.ruleset \
	../../data/nation/nuu-chah-nulth.ruleset \
	../../data/nation/occitan.ruleset \
	../../data/nation/ohlone.ruleset \
	../../data/nation/oldenburgian.ruleset \
	../../data/nation/oldprussian.ruleset \
	../../data/nation/omani.ruleset \
	../../data/nation/ossetian.ruleset \
	../../data/nation/ostrogothic.ruleset \
	../../data/nation/otomi.ruleset \
	../../data/nation/ottoman.ruleset \
	../../data/nation/ozite.ruleset \
	../../data/nation/paeonian.ruleset \
	../../data/nation/pakistani.ruleset \
	../../data/nation/palatinate.ruleset \
	../../data/nation/palestinian.ruleset \
	../../data/nation/palmyrene.ruleset \
	../../data/nation/panamanian.ruleset \
	../../data/nation/papuanewguinean.ruleset \
	../../data/nation/papuan.ruleset \
	../../data/nation/paraguayan.ruleset \
	../../data/nation/parthian.ruleset \
	../../data/nation/pashtun.ruleset \
	../../data/nation/pelasgian.ruleset \
	../../data/nation/peruvian.ruleset \
	../../data/nation/phoenician.ruleset \
	../../data/nation/phrygian.ruleset \
	../../data/nation/pictish.ruleset \
	../../data/nation/piedmontese.ruleset \
	../../data/nation/pontic.ruleset \
	../../data/nation/prussian.ruleset \
	../../data/nation/puertorican.ruleset \
	../../data/nation/purhepecha.ruleset \
	../../data/nation/qatari.ruleset \
	../../data/nation/quebecois.ruleset \
	../../data/nation/rapanui.ruleset \
	../../data/nation/raramuri.ruleset \
	../../data/nation/rhenish.ruleset \
	../../data/nation/riffian.ruleset \
	../../data/nation/riograndense.ruleset \
	../../data/nation/romanian.ruleset \
	../../data/nation/romansh.ruleset \
	../../data/nation/rusyn.ruleset \
	../../data/nation/ruthenian.ruleset \
	../../data/nation/rwandan.ruleset \
	../../data/nation/ryukyuan.ruleset \
	../../data/nation/sabine.ruleset \
	../../data/nation/sahrawi.ruleset \
	../../data/nation/saintlucian.ruleset \
	../../data/nation/saka.ruleset \
	../../data/nation/salishan.ruleset \
	../../data/nation/salvadoran.ruleset \
	../../data/nation/sami.ruleset \
	../../data/nation/sammarinese.ruleset \
	../../data/nation/samnite.ruleset \
	../../data/nation/samoan.ruleset \
	../../data/nation/samogitian.ruleset \
	../../data/nation/santomean.ruleset \
	../../data/nation/sardinian.ruleset \
	../../data/nation/sarmatian.ruleset \
	../../data/nation/saudi.ruleset \
	../../data/nation/savoyard.ruleset \
	../../data/nation/saxon.ruleset \
	../../data/nation/scanian.ruleset \
	../../data/nation/schleswig-holsteinian.ruleset \
	../../data/nation/scottishgaelic.ruleset \
	../../data/nation/scottish.ruleset \
	../../data/nation/scythian.ruleset \
	../../data/nation/seleucid.ruleset \
	../../data/nation/seljuk.ruleset \
	../../data/nation/seminole.ruleset \
	../../data/nation/senegalese.ruleset \
	../../data/nation/serbian.ruleset \
	../../data/nation/seychellois.ruleset \
	../../data/nation/shan.ruleset \
	../../data/nation/shawnee.ruleset \
	../../data/nation/sherpa.ruleset \
	../../data/nation/siberian.ruleset \
	../../data/nation/sicilian.ruleset \
	../../data/nation/sierraleonean.ruleset \
	../../data/nation/sikh.ruleset \
	../../data/nation/sikkimese.ruleset \
	../../data/nation/silesian.ruleset \
	../../data/nation/singaporean.ruleset \
	../../data/nation/sinhalese.ruleset \
	../../data/nation/slavic.ruleset \
	../../data/nation/slovakian.ruleset \
	../../data/nation/slovenian.ruleset \
	../../data/nation/solomonislander.ruleset \
	../../data/nation/somaliland.ruleset \
	../../data/nation/somali.ruleset \
	../../data/nation/sorbian.ruleset \
	../../data/nation/sotho.ruleset \
	../../data/nation/southafrican.ruleset \
	../../data/nation/southamerican.ruleset \
	../../data/nation/southkorean.ruleset \
	../../data/nation/southsudanese.ruleset \
	../../data/nation/southvietnamese.ruleset \
	../../data/nation/southyemeni.ruleset \
	../../data/nation/soviet.ruleset \
	../../data/nation/srivijaya.ruleset \
	../../data/nation/sudanese.ruleset \
	../../data/nation/suebian.ruleset \
	../../data/nation/surinamese.ruleset \
	../../data/nation/swahili.ruleset \
	../../data/nation/swazi.ruleset \
	../../data/nation/swiss.ruleset \
	../../data/nation/syrian.ruleset \
	../../data/nation/tahitian.ruleset \
	../../data/nation/tairona.ruleset \
	../../data/nation/taiwanese.ruleset \
	../../data/nation/tajik.ruleset \
	../../data/nation/tanganyikan.ruleset \
	../../data/nation/tanzanian.ruleset \
	../../data/nation/tatar.ruleset \
	../../data/nation/templar.ruleset \
	../../data/nation/teutonic.ruleset \
	../../data/nation/texan.ruleset \
	../../data/nation/thracian.ruleset \
	../../data/nation/thuringian.ruleset \
	../../data/nation/tibetan.ruleset \
	../../data/nation/timurid.ruleset \
	../../data/nation/tocharian.ruleset \
	../../data/nation/togolese.ruleset \
	../../data/nation/toltec.ruleset \
	../../data/nation/tongan.ruleset \
	../../data/nation/transnistrian.ruleset \
	../../data/nation/transylvanian.ruleset \
	../../data/nation/trinidadian.ruleset \
	../../data/nation/tswana.ruleset \
	../../data/nation/tuareg.ruleset \
	../../data/nation/tunisian.ruleset \
	../../data/nation/turkishcypriot.ruleset \
	../../data/nation/turkmen.ruleset \
	../../data/nation/tuvan.ruleset \
	../../data/nation/tyrolian.ruleset \
	../../data/nation/ugandan.ruleset \
	../../data/nation/ukrainian.ruleset \
	../../data/nation/un.ruleset \
	../../data/nation/urartian.ruleset \
	../../data/nation/uruguayan.ruleset \
	../../data/nation/uyghur.ruleset \
	../../data/nation/uzbek.ruleset \
	../../data/nation/vampire.ruleset \
	../../data/nation/vandalic.ruleset \
	../../data/nation/vanuatuan.ruleset \
	../../data/nation/vedic.ruleset \
	../../data/nation/veletian.ruleset \
	../../data/nation/venda.ruleset \
	../../data/nation/venetian.ruleset \
	../../data/nation/venetic.ruleset \
	../../data/nation/venezuelan.ruleset \
	../../data/nation/vermont.ruleset \
	../../data/nation/vietnamese.ruleset \
	../../data/nation/vincentian.ruleset \
	../../data/nation/visigothic.ruleset \
	../../data/nation/vistulan.ruleset \
	../../data/nation/volapuk.ruleset \
	../../data/nation/volgabulgar.ruleset \
	../../data/nation/volgagerman.ruleset \
	../../data/nation/walloon.ruleset \
	../../data/nation/welsh.ruleset \
	../../data/nation/werewolf.ruleset \
	../../data/nation/westernpomeranian.ruleset \
	../../data/nation/westernroman.ruleset \
	../../data/nation/westindian.ruleset \
	../../data/nation/westphalian.ruleset \
	../../data/nation/wuerttembergian.ruleset \
	../../data/nation/xhosa.ruleset \
	../../data/nation/xiongnu.ruleset \
	../../data/nation/yakut.ruleset \
	../../data/nation/yemeni.ruleset \
	../../data/nation/yucatecan.ruleset \
	../../data/nation/yugoslav.ruleset \
	../../data/nation/zambian.ruleset \
	../../data/nation/zanzibari.ruleset \
	../../data/nation/zapotec.ruleset \
	../../data/nation/zhuang.ruleset \
	../../data/nation/zimbabwean.ruleset

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
