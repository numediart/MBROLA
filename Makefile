# BEGIN_COMM <- flags parenthesize what goes in the commercial Makefile (awk strips)
# BEGIN_WWW  <- flags parenthesize what goes in the Web Makefile (stripped by awk below)
# Mbrola Speech Synthesize Makefile ( tune the #define and type "make" )
VERSION=3.02b

# To test strict C ANSI compliance
CC = gcc -ansi -pedantic
LIB= -lm

# This allow you to write commands like "make PURE=purify demo1"
# or "make PURE=quantify lib2"
CCPURE = $(PURE) $(CC)

########################
# Machine specific #define, uncomment as needed
# If your OS is not here, may be it provides some __linux or __sunos
# that are used in Misc/common.h If compiling fails see that file

#CFLAGS += -DTARGET_OS_DOS
#CFLAGS += -DTARGET_OS_SUN4
#CFLAGS += -DTARGET_OS_VMS
#CFLAGS += -DTARGET_OS_MAC

# See below for BEOS
# CFLAGS += -DTARGET_OS_BEOS

# If endianess of your machine is not automatically detected in Misc/common.h
# you should manually specify here
CFLAGS += -DLITTLE_ENDIAN
#CFLAGS += -DBIG_ENDIAN

#######################
# GENERAL FLAGS FOR GCC

# Optimized code
CFLAGS += -O6

# Debug mode with gdb
#CFLAGS += -g 

# Print MANY debug information on stdout
#CFLAGS += -DDEBUG

# For testing hash_table performances
#CFLAGS += -DDEBUG_HASH

#Include directories
CFLAGS += -IParser -IStandalone -IMisc -ILibOneChannel -ILibMultiChannel -IEngine -IDatabase

# Flags for GCC
CFLAGS += -Wall
# CFLAGS += -fhandle-exceptions

# Flags for OS/2
#CFLAGS += -Zmts -D__ST_MT_ERRNO -Zexe -Zomf -Zsys
#CFLAGS += -Zmtd -D__ST_MT_ERRNO -Zexe -Zomf

# BeOs 3.2
# -w all is too sensible on  prototype
# CC = cc -w all
# -w on make some warning
# CC = cc -w on
# LIB =

# BeOS 4.x
# LIB = -lbe -lroot
# CFLAGS += -O1
# or CFLAGS += -O3

COMMONSRCS = Engine/mbrola.c Engine/diphone.c Parser/phone.c Parser/parser_input.c Parser/input_file.c Parser/phonbuff.c Misc/audio.c Misc/vp_error.c Misc/mbralloc.c Misc/common.c Database/database.c Database/database_old.c Database/diphone_info.c Database/little_big.c Database/hash_tab.c Database/zstring_list.c

COMMONCHDRS = Engine/mbrola.h Engine/diphone.h Parser/phone.h Parser/parser.h Parser/input_file.h Parser/input.h Parser/phonbuff.h Misc/incdll.h Misc/audio.h Misc/vp_error.h Misc/mbralloc.h Misc/common.h Database/database.h Database/database_old.h Database/diphone_info.h Database/little_big.h Database/hash_tab.h Database/phoname_list.h

# END_WWW

# LibOneChannel or LibMultiChannel
#CFLAGS += -DLIBRARY

# DLL is a library with exported symbols
#CFLAGS += -DDLL

######################################################
# ROM SECTION
#

# Uncomment if you want a pure rom access, that is NO FILE AT ALL
# Note that this mode is incompatible with ROMDATABASE_STORE as it
# requires files for this operation !
# CFLAGS += -DROMDATABASE_PURE

# Uncomment if you wish to save .rom images from regular diphone databases
CFLAGS += -DROMDATABASE_STORE

# Uncomment if you wish to use init_ROM_Database(ROM_pointer)
CFLAGS += -DROMDATABASE_INIT

# Uncomment to cope with ROMDATABASE_STORE or ROMDATABASE_INIT
COMMONSRCS += Database/rom_handling.c Database/rom_database.c


######################################################
# DATABASE COMPRESSION SECTION
#

# END_COMM

# BEGIN_WWW

# Signal handling of the standalone version (Unix platforms)
CFLAGS += -DSIGNAL

# Add external cflags
CFLAGS += $(EXT_CFLAGS)

# BEGIN_COMM
BINSRCS = Standalone/synth.c $(COMMONSRCS)
BINHDRS = Standalone/synth.h $(COMMONHDRS)

LIBSRCS = Misc/g711.c Parser/input_fifo.c Parser/fifo.c $(COMMONSRCS)
LIBHDRS = Misc/g711.h Parser/input_fifo.h Parser/fifo.h $(COMMONHDRS)

MBRDIR = ./Bin
BINDIR = ./Bin/Standalone
LIBDIR = ./Bin/LibOneChannel

BINOBJS = $(BINSRCS:%.c=Bin/Standalone/%.o)

PROJ = mbrola

$(PROJ): install_dir  $(BINOBJS) 
	$(CCPURE) $(CFLAGS) -o $(MBRDIR)/$(PROJ) $(BINOBJS) $(LIB)	

clean:
	\rm -f $(MBRDIR)/$(PROJ) $(PROJ).a core demo* TAGS $(BIN)/lib*.o $(BINOBJS) 
	\rm -rf VisualC++/DLL/output VisualC++/DLL/mbroladl VisualC++/DLL/mbroladll.ncb VisualC++/DLL/mbroladll.opt VisualC++/DLL/*.plg .sb
	\rm -rf VisualC++/Standalone/output VisualC++/Standalone/mbroladl VisualC++/Standalone/mbrola.ncb VisualC++/Standalone/mbrola.opt VisualC++/Standalone/*.plg .sb
	\rm -rf  delexsend$(VERSION) send$(VERSION) mbr$(VERSION)
	\rm -rf doc.txt

spotless: clean net
	\rm -rf Bin

tags:
	etags */*{c,h}

net:
	\rm -f *~ */*~

$(BINDIR)/%.o: %.c
	$(CCPURE) $(CFLAGS) -o $@ -c $<

# to create the compilation directory, if necessary
install_dir: 
	if [ ! -d Bin/Standalone ]; then \
	mkdir Bin ; mkdir Bin/LibOneChannel; mkdir Bin/LibMultiChannel ; mkdir Bin/Standalone ; mkdir Bin/Standalone/Standalone ; mkdir Bin/Standalone/Parser ;	mkdir Bin/Standalone/Engine ;	mkdir Bin/Standalone/Database ;	mkdir Bin/Standalone/Misc; \
	fi

lib1 : LibOneChannel/lib1.c
	$(CCPURE) $(CFLAGS) -o Bin/LibOneChannel/lib1.o -c LibOneChannel/lib1.c

demo1:  install_dir lib1 LibOneChannel/demo1.c
	$(CCPURE) $(CFLAGS) -c -o Bin/LibOneChannel/demo1.o LibOneChannel/demo1.c
	$(CCPURE) $(CFLAGS) -o demo1 Bin/LibOneChannel/demo1.o Bin/LibOneChannel/lib1.o $(LIB)		

# END_WWW

demo1b:  install_dir lib1 LibOneChannel/demo1b.c
	$(CCPURE) $(CFLAGS) -c -o Bin/LibOneChannel/demo1b.o LibOneChannel/demo1b.c
	$(CCPURE) $(CFLAGS) -o demo1b Bin/LibOneChannel/demo1b.o Bin/LibOneChannel/lib1.o $(LIB)	


lib2 : LibMultiChannel/lib2.c
	$(CCPURE) $(CFLAGS) -o Bin/LibMultiChannel/lib2.o -c LibMultiChannel/lib2.c

demo2: install_dir lib2 LibMultiChannel/demo2.c	
	$(CCPURE) $(CFLAGS) -c -o Bin/LibMultiChannel/demo2.o LibMultiChannel/demo2.c
	$(CCPURE) $(CFLAGS) -o demo2 Bin/LibMultiChannel/demo2.o Bin/LibMultiChannel/lib2.o $(LIB)		
# END_COMM

# Check the integrity of the new Mbrola version by comparing the output 
# to previous standard output. Worry if you see the "Binary files differ" 
# message
checkold: demo1 demo2 synth
	@ echo ""
	@ echo ""
	@ echo "Check the integrity of the new Mbrola version"

# the code below generates errors -> dedicated to checks with Purify 
	-@./demo1 UTILITY_TCTS
	-@./demo2 UTILITY_TCTS
	-@./demo1 UTILITY_TCTS/vincee1
	-@./demo2 UTILITY_TCTS/vincee1
	-@./demo1 UTILITY_TCTS/us1.cebab
	-@./demo2 UTILITY_TCTS/us1.cebab
# The code below successfully synthesize 
	./synth UTILITY_TCTS/fr1 UTILITY_TCTS/bonjour.pho resbon1.wav
	./synth -R "on o~ j Z" -C "ou u r R" UTILITY_TCTS/vincee1 UTILITY_TCTS/bonjour.pho resbon2.wav
	./demo1 UTILITY_TCTS/fr1
	./demo2 UTILITY_TCTS/fr1
	./synth UTILITY_TCTS/us1.cebab UTILITY_TCTS/alice.pho resalis.au
	@ echo ""
	@ echo ""
	@ echo "Worry if you see the Binary files differ message"
	diff res.ulaw UTILITY_TCTS/$(OSTYPE)
	diff resbon1.wav UTILITY_TCTS/$(OSTYPE)
	diff resbon2.wav UTILITY_TCTS/$(OSTYPE)
	diff resalis.au UTILITY_TCTS/$(OSTYPE)
	diff res1.ulaw UTILITY_TCTS/$(OSTYPE)
	diff res2.raw UTILITY_TCTS/$(OSTYPE)
	diff res3.lin8 UTILITY_TCTS/$(OSTYPE)

check: checkold
# Generate ROM images
	./synth -W UTILITY_TCTS/fr1
	./synth -W UTILITY_TCTS/us1.cebab
	./synth -w UTILITY_TCTS/fr1 UTILITY_TCTS/bonjour.pho resbon1rom.wav
	./synth -w UTILITY_TCTS/us1.cebab UTILITY_TCTS/alice.pho resalisrom.au
	diff resbon1rom.wav resbon1.wav
	diff resalisrom.au resalis.au
	\rm -f res* UTILITY_TCTS/fr1.rom UTILITY_TCTS/us1.cebab.rom

# Put the right version number in common.h
version:
	@ mv Misc/common.h Misc/common.h~
	@ sed 's/SYNTH_VERSION.*/SYNTH_VERSION \"$(VERSION)\"/' < Misc/common.h~ > Misc/common.h; 

# Internet delexicalized version ( Unix standalone mode )
delex: version spotless
	mkdir delexsend$(VERSION) ; cd delexsend$(VERSION) ; mkdir Database Engine Misc Parser Standalone LibOneChannel ; echo "Version $(VERSION) `date`" > readme.txt; cat ../DOCUMENTATION/User/readme.txt >> readme.txt ; cp ../00readme_* . ; cp -r ../Standalone/Posix Standalone/Posix ; cp ../Misc/incdll.h Misc ; ../UTILITY_TCTS/source_delexicalize.pl Database/database.c Database/database.h Database/database_cebab.c Database/database_cebab.h Database/database_old.c Database/database_old.h Database/diphone_info.c Database/diphone_info.h Database/hash_tab.c Database/hash_tab.h Database/little_big.c Database/little_big.h Database/zstring_list.c Database/zstring_list.h Engine/diphone.c Engine/diphone.h Engine/mbrola.c Engine/mbrola.h Misc/audio.c Misc/audio.h Misc/common.c Misc/common.h Misc/g711.c Misc/g711.h Misc/mbralloc.c Misc/mbralloc.h Misc/vp_error.c Misc/vp_error.h Parser/fifo.c Parser/fifo.h Parser/input.h Parser/input_fifo.c Parser/input_fifo.h Parser/input_file.c Parser/input_file.h Parser/parser.h Parser/parser_input.c Parser/parser_input.h Parser/phonbuff.c Parser/phonbuff.h Parser/phone.c Parser/phone.h  Standalone/synth.c Standalone/synth.h LibOneChannel/onechannel.c ; cp ../LibOneChannel/onechannel.h ../LibOneChannel/lib1.c ../LibOneChannel/demo1.c  LibOneChannel ; awk '/# BEGIN_WWW/,/# END_WWW/' < ../Makefile |  sed 's/.*_COMM.*//' |sed 's/.*_WWW.*//' > Makefile

big: delex
	cd delexsend$(VERSION) ; cp ../Misc/incdll.h . ; cp ../LibOneChannel/onechannel.h . ; cp ../Parser/parser.h . ; cp ../Parser/phone_export.h phone.h ; cat Misc/common.h Misc/vp_error.h Misc/mbralloc.h Misc/incdll.h Parser/phone.h Database/diphone_info.h Engine/diphone.h Parser/parser.h Misc/g711.h Misc/audio.h Database/zstring_list.h Database/hash_tab.h Database/little_big.h Database/database.h Database/database_cebab.h  Engine/mbrola.h Database/database_old.h Parser/input.h Parser/fifo.h Parser/phonbuff.h Parser/parser_input.h Parser/input_fifo.h LibOneChannel/onechannel.h Misc/mbralloc.c Database/little_big.c Misc/common.c Parser/phone.c Engine/diphone.c Misc/g711.c Misc/audio.c Engine/mbrola.c Database/diphone_info.c Database/database_old.c Database/database.c Database/zstring_list.c Parser/phonbuff.c  Parser/fifo.c Parser/input_fifo.c Parser/parser_input.c  Database/hash_tab.c LibOneChannel/onechannel.c Misc/vp_error.c Database/database_cebab.c | sed 's/#include ".*//g' > mbrola_lib.c ; \rm -rf Database Engine Misc Parser Standalone LibOneChannel

# Internet public version (no ROM, no DEBUG and no MULTI_CHANNEL)
send: version spotless
	mkdir send$(VERSION) ; cd send$(VERSION) ; cp ../00readme_* . ; mkdir Database Engine Misc Parser Standalone LibOneChannel ; echo "Version $(VERSION) `date`" > readme.txt; cat ../DOCUMENTATION/User/readme.txt >> readme.txt ; cp -r ../Standalone/Posix Standalone/Posix ; ../UTILITY_TCTS/source_clean_public.pl Database/database.c Database/database.h Database/database_cebab.c Database/database_cebab.h Database/database_old.c Database/database_old.h Database/diphone_info.c Database/diphone_info.h Database/hash_tab.c Database/hash_tab.h Database/little_big.c Database/little_big.h Database/zstring_list.c Database/zstring_list.h Engine/diphone.c Engine/diphone.h Engine/mbrola.c Engine/mbrola.h Misc/audio.c Misc/audio.h Misc/common.c Misc/common.h Misc/g711.c Misc/g711.h Misc/mbralloc.c Misc/mbralloc.h Misc/vp_error.c Misc/vp_error.h Parser/fifo.c Parser/fifo.h Parser/input.h Parser/input_fifo.c Parser/input_fifo.h Parser/input_file.c Parser/input_file.h Parser/parser.h Parser/parser_input.c Parser/parser_input.h Parser/phonbuff.c Parser/phonbuff.h Parser/phone.c Parser/phone.h  Standalone/synth.c Standalone/synth.h LibOneChannel/onechannel.c LibOneChannel/onechannel.h LibOneChannel/lib1.c LibOneChannel/demo1.c Misc/incdll.h ; awk '/# BEGIN_WWW/,/# END_WWW/' < ../Makefile |  sed 's/.*_COMM.*//' |sed 's/.*_WWW.*//' > Makefile

# Commercial Version
com: version spotless
	mkdir mbr$(VERSION) ; cd mbr$(VERSION) ; cp -r ../Database . ; cp -r ../Engine . ; cp -r ../LibMultiChannel . ;cp -r ../LibOneChannel . ;cp -r ../Misc . ;cp -r ../Parser . ;cp -r ../Standalone . ;cp -r ../VisualC++ . ;cp -r ../DOCUMENTATION . ; echo "Version $(VERSION) `date`" > DOCUMENTATION/User/readme.txt; cat ../DOCUMENTATION/User/readme.txt >> DOCUMENTATION/User/readme.txt ; \rm -f Database/database_cebab.* ; awk '/# BEGIN_COMM/,/# END_COMM/' < ../Makefile | sed 's/.*_COMM.*//' | sed 's/.*_WWW.*//'> Makefile ; sed 's/RELEASE/$(VERSION)/g' < ../00readme.txt > 00readme.txt

sauve: spotless
	cd ..; gtar cvfz SAUVE/synth$(VERSION).tgz synth$(VERSION)/

doc: 
	echo "File: Misc/common.h" >> doc.txt
	cat Misc/common.h >> doc.txt
#	echo "File: Misc/common.c" >> doc.txt
#	cat Misc/common.c| awk '/{/ { nb++; } { if (nb == 0) print; } /}/ { nb--; }' >> doc.txt
	echo "File: Misc/incdll.h" >> doc.txt
	cat Misc/incdll.h >> doc.txt
	echo "File: Misc/mbralloc.h" >> doc.txt
	cat Misc/mbralloc.h >> doc.txt
#	echo "File: Misc/mbralloc.c" >> doc.txt
#	cat Misc/mbralloc.c| awk '/{/ { nb++; } { if (nb == 0) print; } /}/ { nb--; }' >> doc.txt
	echo "File: Misc/vp_error.h" >> doc.txt
	cat Misc/vp_error.h >> doc.txt
#	echo "File: Misc/vp_error.c" >> doc.txt
#	cat Misc/vp_error.c| awk '/{/ { nb++; } { if (nb == 0) print; } /}/ { nb--; }' >> doc.txt
	echo "File: Misc/audio.h" >> doc.txt
	cat Misc/audio.h >> doc.txt
#	echo "File: Misc/audio.c" >> doc.txt
#	cat Misc/audio.c| awk '/{/ { nb++; } { if (nb == 0) print; } /}/ { nb--; }' >> doc.txt
	echo "File: Database/database.h" >> doc.txt
	cat Database/database.h >> doc.txt
	echo "File: Database/database.c " >> doc.txt
#	cat Database/database.c|  awk '/{/ { nb++; } { if (nb == 0) print; } /}/ { nb--; }' >> doc.txt
	echo "File: Database/rom_database.h" >> doc.txt
	cat Database/rom_database.h >> doc.txt
	echo "File: Database/rom_database.c " >> doc.txt
#	cat Database/rom_database.c|  awk '/{/ { nb++; } { if (nb == 0) print; } /}/ { nb--; }' >> doc.txt
	echo "File: Database/rom_handling.h" >> doc.txt
	cat Database/rom_handling.h >> doc.txt
	echo "File: Database/rom_handling.c " >> doc.txt
#	cat Database/rom_handling.c|  awk '/{/ { nb++; } { if (nb == 0) print; } /}/ { nb--; }' >> doc.txt
	echo "File: atabase/database_bacon.h" >> doc.txt
	cat Database/database_bacon.h >> doc.txt
#	echo "File: Database/database_bacon.c" >> doc.txt
#	cat Database/database_bacon.c| awk '/{/ { nb++; } { if (nb == 0) print; } /}/ { nb--; }' >> doc.txt
	echo "File: Database/database_old.h" >> doc.txt
	cat Database/database_old.h >> doc.txt
#	echo "File: Database/database_old.c" >> doc.txt
#	cat Database/database_old.c| awk '/{/ { nb++; } { if (nb == 0) print; } /}/ { nb--; }' >> doc.txt
	echo "File: Database/diphone_info.h" >> doc.txt
	cat Database/diphone_info.h >> doc.txt
#	echo "File: Database/diphone_info.c" >> doc.txt
#	cat Database/diphone_info.c| awk '/{/ { nb++; } { if (nb == 0) print; } /}/ { nb--; }' >> doc.txt
	echo "File: Database/hash_tab.h" >> doc.txt
	cat Database/hash_tab.h >> doc.txt
#	echo "File: Database/hash_tab.c" >> doc.txt
#	cat Database/hash_tab.c| awk '/{/ { nb++; } { if (nb == 0) print; } /}/ { nb--; }' >> doc.txt
	echo "File: Database/little_big.h" >> doc.txt
	cat Database/little_big.h >> doc.txt
#	echo "File: Database/little_big.c" >> doc.txt
#	cat Database/little_big.c| awk '/{/ { nb++; } { if (nb == 0) print; } /}/ { nb--; }' >> doc.txt
	echo "File: Database/zstring_list.h" >> doc.txt
	cat Database/zstring_list.h >> doc.txt
#	echo "File: Database/zstring_list.c" >> doc.txt
#	cat Database/zstring_list.c| awk '/{/ { nb++; } { if (nb == 0) print; } /}/ { nb--; }' >> doc.txt
	echo "File: Engine/diphone.h" >> doc.txt
	cat Engine/diphone.h >> doc.txt
#	echo "File: Engine/diphone.c" >> doc.txt
#	cat Engine/diphone.c| awk '/{/ { nb++; } { if (nb == 0) print; } /}/ { nb--; }' >> doc.txt
	echo "File: Engine/mbrola.h" >> doc.txt
	cat Engine/mbrola.h >> doc.txt
#	echo "File: Engine/mbrola.c" >> doc.txt
#	cat Engine/mbrola.c| awk '/{/ { nb++; } { if (nb == 0) print; } /}/ { nb--; }' >> doc.txt
	echo "File: Parser/fifo.h" >> doc.txt
	cat Parser/fifo.h >> doc.txt
#	echo "File: Parser/fifo.c" >> doc.txt
#	cat Parser/fifo.c| awk '/{/ { nb++; } { if (nb == 0) print; } /}/ { nb--; }' >> doc.txt
	echo "File: Parser/input.h" >> doc.txt
	cat Parser/input.h >> doc.txt
	echo "File: Parser/input_fifo.h" >> doc.txt
	cat Parser/input_fifo.h >> doc.txt
#	echo "File: Parser/input_fifo.c" >> doc.txt
#	cat Parser/input_fifo.c| awk '/{/ { nb++; } { if (nb == 0) print; } /}/ { nb--; }' >> doc.txt
	echo "File: Parser/input_file.h" >> doc.txt
	cat Parser/input_file.h >> doc.txt
#	echo "File: Parser/input_file.c" >> doc.txt
#	cat Parser/input_file.c| awk '/{/ { nb++; } { if (nb == 0) print; } /}/ { nb--; }' >> doc.txt
	echo "File: Parser/parser.h" >> doc.txt
	cat Parser/parser.h >> doc.txt
	echo "File: Parser/parser_input.h" >> doc.txt
	cat Parser/parser_input.h >> doc.txt
#	echo "File: Parser/parser_input.c" >> doc.txt
#	cat Parser/parser_input.c| awk '/{/ { nb++; } { if (nb == 0) print; } /}/ { nb--; }' >> doc.txt
	echo "File: Parser/phonbuff.h" >> doc.txt
	cat Parser/phonbuff.h >> doc.txt
#	echo "File: Parser/phonbuff.c" >> doc.txt
#	cat Parser/phonbuff.c| awk '/{/ { nb++; } { if (nb == 0) print; } /}/ { nb--; }' >> doc.txt
	echo "File: Parser/phone.h" >> doc.txt
	cat Parser/phone.h >> doc.txt
#	echo "File: Parser/phone.c" >> doc.txt
#	cat Parser/phone.c| awk '/{/ { nb++; } { if (nb == 0) print; } /}/ { nb--; }' >> doc.txt
	echo "File: Standalone/synth.h" >> doc.txt
	cat Standalone/synth.h >> doc.txt
#	echo "File: Standalone/synth.c" >> doc.txt
#	cat Standalone/synth.c| awk '/{/ { nb++; } { if (nb == 0) print; } /}/ { nb--; }' >> doc.txt
	echo "File: LibOneChannel/onechannel.h" >> doc.txt
	cat LibOneChannel/onechannel.h >> doc.txt
#	echo "File: LibOneChannel/onechannel.c" >> doc.txt
#	cat LibOneChannel/onechannel.c| awk '/{/ { nb++; } { if (nb == 0) print; } /}/ { nb--; }' >> doc.txt
	echo "File: LibMultiChannel/multichannel.h" >> doc.txt
	cat LibMultiChannel/multichannel.h >> doc.txt
#	echo "File: LibMultiChannel/multichannel.c" >> doc.txt
#	cat LibMultiChannel/multichannel.c| awk '/{/ { nb++; } { if (nb == 0) print; } /}/ { nb--; }' >> doc.txt
