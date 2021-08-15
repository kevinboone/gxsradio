NAME      := gxsradio
VERSION   := 0.1b
CC        :=  gcc 
GTK_LIBS  := -lpango-1.0 -lgdk-3 -lgdk_pixbuf-2.0 -lgtk-3 -lgobject-2.0 -lglib-2.0 ${EXTRA_LIBS} 
LIBS      := ${GTK_LIBS} ${EXTRA_LIBS} 
GTK_INC   := -I /usr/include/gtk-3.0 -I /usr/include/harfbuzz
GLIB_INC  := -I /usr/include/glib-2.0 -I /usr/lib64/glib-2.0/include
PANGO_INC := -I /usr/include/pango-1.0
CAIRO_INC := -I /usr/include/cairo
GDK_INC   := -I /usr/include/gdk-pixbuf-2.0
ATK_INC   := -I /usr/include/atk-1.0
TARGET	  := $(NAME)
SOURCES   := $(shell find src/ -type f -name *.c)
OBJECTS   := $(patsubst src/%,build/%,$(SOURCES:.c=.o))
DEPS	  := $(OBJECTS:.o=.deps)
DESTDIR   := /
PREFIX    := /usr
MANDIR    := $(DESTDIR)/$(PREFIX)/share/man
BINDIR    := $(DESTDIR)/$(PREFIX)/bin
SHAREBASE := $(DESTDIR)/$(PREFIX)/share/
SHARE     := $(DESTDIR)/$(PREFIX)/share/$(TARGET)
CFLAGS    := -g -fpie -fpic -Wall -DNAME=\"$(NAME)\" -DVERSION=\"$(VERSION)\" -DSHARE=\"$(SHARE)\" -DPREFIX=\"$(PREFIX)\" ${ATK_INC} ${GDK_INC} ${CAIRO_INC} ${PANGO_INC} ${GTK_INC} ${GLIB_INC} ${EXTRA_CFLAGS}
LDFLAGS := -pie ${EXTRA_LDFLAGS}

all: $(TARGET)
debug: CFLAGS += -g
debug: $(TARGET) 

$(TARGET): $(OBJECTS) 
	$(CC) $(LDFLAGS) -o $(TARGET) $(OBJECTS) $(LIBS) 

build/%.o: src/%.c
	@mkdir -p build/
	$(CC) $(CFLAGS) -MD -MF $(@:.o=.deps) -c -o $@ $<

clean:
	@echo "  Cleaning..."; $(RM) -r build/ $(TARGET) 

install: $(TARGET)
	mkdir -p $(DESTDIR)/$(PREFIX) $(DESTDIR)/$(BINDIR) $(DESTDIR)/$(MANDIR)
	mkdir -p $(SHARE)
	strip $(TARGET)
	install -m 755 $(TARGET) $(DESTDIR)/${BINDIR}
	install -m 755 share/*.pl $(SHARE)/
	install -m 644 share/*.gxsradio $(SHARE)/
	install -m 644 share/*.css $(SHARE)/

install-desktop: install 
	install -m 644 share/*.desktop $(SHAREBASE)/applications/
	install -m 644 icons/* $(SHAREBASE)/icons/hicolor/48x48/apps/
	install -m 644 mime/* $(SHAREBASE)/mime/packages/
	update-mime-database $(SHAREBASE)/mime
	#It might be necessary to add the MIME type to mimeapps.list 

-include $(DEPS)

.PHONY: clean

