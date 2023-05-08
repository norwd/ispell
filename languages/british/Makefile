#
# $Id: Makefile,v 1.11 2015-02-08 16:42:36-08 geoff Exp $
#
# Copyright 1994, 2001, 2015, Geoff Kuenning, Claremont, CA
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
# 3. All modifications to the source code must be clearly marked as
#    such.  Binary redistributions based on modified source code
#    must be clearly marked as modified versions in the documentation
#    and/or other materials provided with the distribution.
# 4. The code that causes the 'ispell -v' command to display a prominent
#    link to the official ispell Web site may not be removed.
# 5. The name of Geoff Kuenning may not be used to endorse or promote
#    products derived from this software without specific prior
#    written permission.
#
# THIS SOFTWARE IS PROVIDED BY GEOFF KUENNING AND CONTRIBUTORS ``AS IS'' AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED.  IN NO EVENT SHALL GEOFF KUENNING OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
# OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
# HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
# OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
# SUCH DAMAGE.
#
# This Makefile allows a single installation to build an British
# variant of ispell's English dictionary.  It depends on the
# languages/english directory for most of its input files.
#
# $Log: Makefile,v $
# Revision 1.11  2015-02-08 16:42:36-08  geoff
# Support DESTDIR.  Get rid of the old munchlist-suppressing code and
# references to english.5.  Use symbolic links instead of hard links;
# symbolic links are safer.
#
# Revision 1.10  2015-02-08 01:11:55-08  geoff
# Support DESTDIR.
#
# Revision 1.9  2005-04-14 14:27:35-07  geoff
# Build english.5 as part of installation.
#
# Revision 1.8  2005/04/14 13:25:20  geoff
# Update license.  Drop unsq.  Changes from Ed Avis: Allow LIBDIR to be
# a relative path.  Allow symbolic links as a configuration option.
#
# Revision 1.7  2001/07/25 21:51:48  geoff
# *** empty log message ***
#
# Revision 1.6  2001/07/23 20:43:38  geoff
# *** empty log message ***
#
# Revision 1.5  1999/01/07 01:23:20  geoff
# Update the copyright.  Get rid of the old shar-based dictioary building.
#
# Revision 1.4  1995/01/08  23:24:18  geoff
# Remember to set SHELLDEBUG when making dictcomponents.
#
# Revision 1.3  1994/08/31  05:58:54  geoff
# Create directories before installing into them, and be sure to set the
# proper modes on manual pages.
#
# Revision 1.2  1994/05/17  06:37:42  geoff
# Explicitly specify the affix files with a path so that initial
# installation will work correctly.
#
# Revision 1.1  1994/04/27  02:45:55  geoff
# Initial revision
#
#

SHELL = /bin/sh
MAKE = make

CONFIG		=	../../config.sh
PATHADDER	=	../..
BUILDHASH	=	../../buildhash

# The following variables should be set by the superior Makefile,
# based on the LANGUAGES variable in config.X.
#
# There are four progressively-larger English dictionaries distributed
# with ispell.  These are named english.sml, english.med, english.lrg,
# and english.xlg.  For each of these, you can also build a "plus"
# version (english.sml+, etc.)  which is created by combining the
# distributed version with an "extra" dictionary defined by EXTRADICT,
# usually /usr/share/dict/words.  The "plus" versions of dictionaries
# require lots of time and temporary file space; make sure you have
# set TMPDIR appropriately.
#
# The dictionaries to be built are listed in the MASTERDICTS variable,
# separated by spaces.  The hash files to be built (and installed) are
# listed in the HASHFILES variable.  Hash files are named as
# "british<xxx>.hash", where <xxx> is the suffix of dictionary (e.g.,
# "med+").  The first-listed hash file will also be installed under
# the name "british.hash".  As a general rule, the dictionaries
# needed to build the HASHFILES must be listed in the MASTERDICTS
# variable.
#
# If you change AFFIXES for english, you should consider also changing
# DEFLANG (in config.X) to match.
#
MASTERDICTS	=	Use_LANGUAGES_from_config.X
HASHFILES	=	Use_LANGUAGES_from_config.X
EXTRADICT	=	Use_LANGUAGES_from_config.X

#
# The following variables may be overridden by the superior Makefile,
# based on the LANGUAGES variable in config.X.
#
AFFIXES	=	english.aff
LANGUAGE =	british

#
# DICTSRC specifies where the dictionary sources are found.  You
# should not have to change this.
#
DICTSRC =	../english

#
#	Set this to "-vx" in the make command line if you need to
#	debug the complex shell commands.
#
SHELLDEBUG = +vx

all:  $(CONFIG)
	@set $(SHELLDEBUG); \
	if [ ! -r english.0 ]; \
	then \
	    $(MAKE) SHELLDEBUG=$(SHELLDEBUG) dictcomponents; \
	else \
	    : ; \
	fi
	@set $(SHELLDEBUG); \
	for dict in $(MASTERDICTS); do \
	    if [ ! -r $$dict ]; \
	    then \
		$(MAKE) \
		  'EXTRADICT=$(EXTRADICT)' 'DICTSRC=$(DICTSRC)' \
		  SHELLDEBUG=$(SHELLDEBUG) $$dict; \
	    else \
		: ; \
	    fi; \
	done
	$(MAKE) SHELLDEBUG=$(SHELLDEBUG) $(HASHFILES)

# Note the fooling around with LIBDIR.  It might be a relative path,
# relative to the top of the ispell source tree.  So we have to cd to
# ../.. before referring to $LIBDIR.  There must be a better way...
# 
install:	all $(CONFIG)
	@. $(CONFIG); \
	  set -x; \
	  cd ../..; \
	  [ -d $(DESTDIR)$$LIBDIR ]  || \
	    $(MAKE) -f Makefile NEWDIR=$(DESTDIR)$$LIBDIR mkdirpath; \
	  cd $(DESTDIR)$$LIBDIR; rm -f english.aff $(HASHFILES) $(LANGUAGE).hash
	@. $(CONFIG); \
	  set -x; \
	  cp $(DICTSRC)/english.aff $(HASHFILES) \
	    `cd ../..; cd $(DESTDIR)$$LIBDIR; pwd`
	@. $(CONFIG); \
	  set -x; \
	  cd ../..; cd $(DESTDIR)$$LIBDIR; \
	  chmod 644 english.aff $(HASHFILES); \
	  for i in $(HASHFILES); do \
	    $$LINK -s $$i $(LANGUAGE).hash; \
	    break; \
	  done

#
#	Dependencies to build extra hash files
#
allhashes:	normhashes plushashes
normhashes:	britishsml.hash britishmed.hash
normhashes:	britishlrg.hash britishxlg.hash
plushashes:	britishsml+.hash britishmed+.hash
plushashes:	britishlrg+.hash britishxlg+.hash

britishsml.hash:	$(BUILDHASH) $(DICTSRC)/$(AFFIXES) british.sml
	rm -f britishsml.hash
	$(BUILDHASH) british.sml $(DICTSRC)/$(AFFIXES) britishsml.hash

britishsml+.hash:	$(BUILDHASH) $(DICTSRC)/$(AFFIXES) british.sml+
	rm -f britishsml+.hash
	$(BUILDHASH) british.sml+ $(DICTSRC)/$(AFFIXES) britishsml+.hash

britishmed.hash:	$(BUILDHASH) $(DICTSRC)/$(AFFIXES) british.med
	rm -f britishmed.hash
	$(BUILDHASH) british.med $(DICTSRC)/$(AFFIXES) britishmed.hash

britishmed+.hash:	$(BUILDHASH) $(DICTSRC)/$(AFFIXES) british.med+
	rm -f britishmed+.hash
	$(BUILDHASH) british.med+ $(DICTSRC)/$(AFFIXES) britishmed+.hash

britishlrg.hash:	$(BUILDHASH) $(DICTSRC)/$(AFFIXES) british.lrg
	rm -f britishlrg.hash
	$(BUILDHASH) british.lrg $(DICTSRC)/$(AFFIXES) britishlrg.hash

britishlrg+.hash:	$(BUILDHASH) $(DICTSRC)/$(AFFIXES) british.lrg+
	rm -f britishlrg+.hash
	$(BUILDHASH) british.lrg+ $(DICTSRC)/$(AFFIXES) britishlrg+.hash

britishxlg.hash:	$(BUILDHASH) $(DICTSRC)/$(AFFIXES) british.xlg
	rm -f britishxlg.hash
	$(BUILDHASH) british.xlg $(DICTSRC)/$(AFFIXES) britishxlg.hash

britishxlg+.hash:	$(BUILDHASH) $(DICTSRC)/$(AFFIXES) british.xlg+
	rm -f britishxlg+.hash
	$(BUILDHASH) british.xlg+ $(DICTSRC)/$(AFFIXES) britishxlg+.hash

#
#	The eight dictionaries, british.sml through british.xlg+, are
#	built by the following dependencies.  We used to use some
#	Makefile tricks to avoid running munchlist unnecessarily, but
#	that leads to mistakes, and modern machines run munchlist fast
#	enough that it's no longer worth suppressing.
#
alldicts:	normdicts plusdicts
normdicts:	british.sml
normdicts:	british.med
normdicts:	british.lrg
normdicts:	british.xlg
plusdicts:	british.sml+
plusdicts:	british.med+
plusdicts:	british.lrg+
plusdicts:	british.xlg+

british.sml:	english.sml
	rm -f british.sml
	@. $(CONFIG); \
	  set -x; \
	  $$LINK -s english.sml british.sml

english.sml:	$(CONFIG)
english.sml:	english.0
english.sml:	british.0
	$(MAKE) -f $(DICTSRC)/Makefile VARIANTS=british \
	  'EXTRADICT=$(EXTRADICT)' 'SHELLDEBUG=$(SHELLDEBUG)' \
	  'AFFIXES=$(DICTSRC)/$(AFFIXES)' \
	  english.sml

british.sml+:	english.sml+
	rm -f british.sml+
	@. $(CONFIG); \
	  set -x; \
	  $$LINK -s english.sml+ british.sml+

english.sml+:	$(CONFIG)
english.sml+:	english.0
english.sml+:	british.0
	$(MAKE) -f $(DICTSRC)/Makefile VARIANTS=british \
	  'EXTRADICT=$(EXTRADICT)' 'SHELLDEBUG=$(SHELLDEBUG)' \
	  'AFFIXES=$(DICTSRC)/$(AFFIXES)' \
	  english.sml+

british.med:	english.med
	rm -f british.med
	@. $(CONFIG); \
	  set -x; \
	  $$LINK -s english.med british.med

english.med:	$(CONFIG)
english.med:	english.0
english.med:	british.0
	$(MAKE) -f $(DICTSRC)/Makefile VARIANTS=british \
	  'EXTRADICT=$(EXTRADICT)' 'SHELLDEBUG=$(SHELLDEBUG)' \
	  'AFFIXES=$(DICTSRC)/$(AFFIXES)' \
	  english.med

british.med+:	english.med+
	rm -f british.med+
	@. $(CONFIG); \
	  set -x; \
	  $$LINK -s english.med+ british.med+

english.med+:	$(CONFIG)
english.med+:	english.0
english.med+:	british.0
	$(MAKE) -f $(DICTSRC)/Makefile VARIANTS=british \
	  'EXTRADICT=$(EXTRADICT)' 'SHELLDEBUG=$(SHELLDEBUG)' \
	  'AFFIXES=$(DICTSRC)/$(AFFIXES)' \
	  english.med+

british.lrg:	english.lrg
	rm -f british.lrg
	@. $(CONFIG); \
	  set -x; \
	  $$LINK -s english.lrg british.lrg

english.lrg:	$(CONFIG)
english.lrg:	english.0
english.lrg:	british.0
	$(MAKE) -f $(DICTSRC)/Makefile VARIANTS=british \
	  'EXTRADICT=$(EXTRADICT)' 'SHELLDEBUG=$(SHELLDEBUG)' \
	  'AFFIXES=$(DICTSRC)/$(AFFIXES)' \
	  english.lrg

british.lrg+:	english.lrg+
	rm -f british.lrg+
	@. $(CONFIG); \
	  set -x; \
	  $$LINK -s english.lrg+ british.lrg+

english.lrg+:	$(CONFIG)
english.lrg+:	english.0
english.lrg+:	british.0
	$(MAKE) -f $(DICTSRC)/Makefile VARIANTS=british \
	  'EXTRADICT=$(EXTRADICT)' 'SHELLDEBUG=$(SHELLDEBUG)' \
	  'AFFIXES=$(DICTSRC)/$(AFFIXES)' \
	  english.lrg+

british.xlg:	english.xlg
	rm -f british.xlg
	@. $(CONFIG); \
	  set -x; \
	  $$LINK -s english.xlg british.xlg

english.xlg:	$(CONFIG)
english.xlg:	english.0
english.xlg:	british.0
	$(MAKE) -f $(DICTSRC)/Makefile VARIANTS=british \
	  'EXTRADICT=$(EXTRADICT)' 'SHELLDEBUG=$(SHELLDEBUG)' \
	  'AFFIXES=$(DICTSRC)/$(AFFIXES)' \
	  english.xlg

british.xlg+:	english.xlg+
	rm -f british.xlg+
	@. $(CONFIG); \
	  set -x; \
	  $$LINK -s english.xlg+ british.xlg+

english.xlg+:	$(CONFIG)
english.xlg+:	english.0
english.xlg+:	british.0
	$(MAKE) -f $(DICTSRC)/Makefile VARIANTS=british \
	  'EXTRADICT=$(EXTRADICT)' 'SHELLDEBUG=$(SHELLDEBUG)' \
	  'AFFIXES=$(DICTSRC)/$(AFFIXES)' \
	  english.xlg+

#
#	Dependencies to create the components of the dictionaries,
#	preferably by linking.  We create some components that we'll
#	never use because the master English Makefile requires them.
#
dictcomponents:	english.0
dictcomponents:	english.1
dictcomponents:	english.2
dictcomponents:	english.3
dictcomponents: american.0
dictcomponents: american.1
dictcomponents:	american.2
dictcomponents: altamer.0
dictcomponents: altamer.1
dictcomponents:	altamer.2
dictcomponents: british.0
dictcomponents: british.1
dictcomponents:	british.2

english.0 english.1 english.2 english.3 \
  american.0 american.1 american.2 \
  altamer.0 altamer.1 altamer.2 \
  british.0 british.1 british.2:
	rm -f english.[0-3] american.[0-2] altamer.[012] british.[012]
	@set $(SHELLDEBUG); \
	. $(CONFIG); \
	for i in english.0 english.1 english.2 english.3 \
	  american.0 american.1 american.2 altamer.0 altamer.1 altamer.2 \
	  british.0 british.1 british.2; do \
	    $$LINK -s $(DICTSRC)/$$i .  || $$LINK $(DICTSRC)/$$i . \
	      ||  cp $(DICTSRC)/$$i .; \
	done

clean:
	rm -f core *.hash *.stat *.cnt

#
#	The following target allows you to clean out the combined
#	dictionary files.
#
dictclean:
	rm -f english.[0-3] american.[0-3] altamer.[0-3] british.[0-3]
	rm -f *.sml *.sml+ *.med *.med+ *.lrg *.lrg+ *.xlg *.xlg+
