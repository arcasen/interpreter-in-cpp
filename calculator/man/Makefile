# Makefile for merging Markdown files and converting Markdown to PDF, HTML, TEX etc
# Version: 20251016

# Specify input and output directories
DOCS ?= docs
DIST ?= dist

# Changes of defaults.yaml or metadata.yaml will trigger re-compiling
SETTINGS := defaults.yaml metadata.yaml preamble.tex

# Copy all files to the '$(DIST)' directory, where .md files will have their 
# extensions changed to .markdown. Later, rules will be defined to convert 
# these .markdown files into the final .md files.
DOCUMENTS := $(shell find $(DOCS) -not -path $(DOCS)) # everything under $(DOCS)
DUPLICATE := $(patsubst $(DOCS)%,$(DIST)%,$(subst .md,.markdown,$(DOCUMENTS)))
DUPLICATE += $(patsubst %,$(DIST)/%,$(SETTINGS))
MARKDOWNS := $(patsubst $(DOCS)%.md,$(DIST)%.markdown,$(shell find $(DOCS) -name "*.md"))

# Proccessed .md documents (will be created in the directory '$(DIST)')
TARGETS   := $(patsubst $(DIST)/%,%,$(patsubst %.markdown,%.md,$(MARKDOWNS)))

# Top level documents
TOP_DOCS  := $(wildcard $(DOCS)/*.md)

# PDF documents (will be created in the directory '$(DIST)')
PDFS := $(patsubst $(DOCS)/%,%,$(patsubst %.md,%.pdf,$(TOP_DOCS)))

# Tex documents (will be created in the directory '$(DIST)')
TEXS := $(patsubst $(DOCS)/%,%,$(patsubst %.md,%.tex,$(TOP_DOCS)))

# HTML documents (will be created in the directory '$(DIST)')
HTML := $(patsubst $(DOCS)/%,%,$(patsubst %.md,%.html,$(TOP_DOCS)))

# Don't export variables, evaluate them directly in submakefile
# export TARGETS
# export PDFS

# Prerequisites to build targets
requisite: $(DUPLICATE) $(DIST)/Makefile $(DIST)/depends
	make -C $(DIST)

# Create PDF documents
pdf: requisite
	make -C $(DIST) pdf

# Create LaTeX documents
tex: requisite
	make -C $(DIST) tex

# Create html documents
html: requisite
	make -C $(DIST) html

clean:
	make -C $(DIST) clean

cleanup:
	rm -rf $(DIST)

.PHONY: requisite clean cleanup pdf tex html

# Rule: create Makefile in the directory '$(DIST)'
$(DIST)/Makefile: $(MARKDOWNS)
	@echo "Creating $@ ..."
	@mkdir -p $(DIST)
	@echo "$$submakefile" > $@

# Rule: automatically generating inclusion dependencies between Markdown files
$(DIST)/depends: $(MARKDOWNS)
	@echo "Creating $@ ..."
	@mkdir -p $(DIST)
	@$(makedepend)

# Rule: copy Markdown files, change extension and resolve paths
$(DIST)/%.markdown: $(DOCS)/%.md
	@echo "Creating $@ ..."
	@mkdir -p $(dir $@)
	@cp -rf $< $@
	@$(call rebasepath,$@)

# Rule: copy other files to the directory '$(DIST)'
$(DIST)/%: %
	@echo "Creating $@ ..."
	@mkdir -p $(DIST)
	@cp -f $< $@

$(DIST)/%: $(DOCS)/%
	@echo "Creating $@ ..."
	@mkdir -p $(dir $@)
	@cp -rf $< $@

# Rebasing paths of embedded images and markdowns
# Caution: processing embedded images in both inline and reference form
#          - ![<alt-text>](<url> "<picture-title>")
#          - ![<atl-text>][<picture-reference>]
#            [<pciture-reference>]: <url> "<picture-title>"
define rebasepath
$(eval RELPATH := $(patsubst $(DIST)/%,%,$(dir $1)))
sed -i -E -e "/\!\[([^]]*)\]\(\ *(http|https|ftp):\/\/[^)]*\)/b" \
-e "/\!\[([^]]*)\]\(\ *(\/|\~)[^)]*\)/b"                         \
-e "s|\!\[([^]]*)\]\(\ *([^)]*)\)|\![\1]($(RELPATH)\2)|g"        \
-e "/^\!\[\[\ *(\/|\~)[^]]*\]\]$$/b"                             \
-e "s|^\!\[\[\ *([^]]*)\]\]$$|![[$(RELPATH)\1]]|g" $1            \
-e "/\[([^]]*)\]\:\ *(http|https|ftp):\/\/([^[:space:]$$])*/b"   \
-e "/\[([^]]*)\]\:\ *(\/|\~)([^[:space:]$$])*/b"                 \
-e "s|\[([^]]*)\]\:\ *([^[:space:]$$]*)(jpg\|jpeg\|png)|[\1]\: $(RELPATH)\2\3|g"        
endef

# automatically generating inclusion dependencies between Markdown files
define makedepend
find $(DIST) -name "*.markdown" -exec          \
awk 'match($$0, /^!\[\[([^]]+)\]\] *$$/, a) {  \
  gsub(/^ +| +$$/, "", a[1]);                  \
  if (!seen[a[1]]++) {                         \
    files = (files ? files " " : "") a[1]      \
  }                                            \
}                                              \
END {                                          \
  if (files) {                                 \
    sub(/^$(DIST)\//, "", FILENAME);           \
    sub(/\.markdown$$/, ".md", FILENAME);      \
    print FILENAME ": " files                  \
  }                                            \
}' {} \;                                       \
> $(DIST)/depends
endef

# https://stackoverflow.com/questions/649246/is-it-possible-to-create-a-multi-line-string-variable-in-a-makefile
# It is almost impossible to use a heredoc with tab indentation in a Makefile;
# additionally, when it comes to the number of dollar signs below variables, 
# the actual number of dollar signs output is only half.
define submakefile
# TARGETS is defined in parent Makefile
all: $(TARGETS) 

pdf: $(PDFS)

tex: $(TEXS)

html: $(HTML)

clean:
	rm -f $(PDFS) $(TEXS) $(HTML)

.PHONY: all clean

include depends

# Rule: perform file inclusion
%.md: %.markdown
	@echo "Processing $$< ..."
	@perl -ne 's/^!\[\[(.+)\]\]\ *$$$$/`cat $$$$1`/e;print' $$< > $$@

# Rule: create PDF document
# Solve the issue of the color on both sides of a striped table exceeding the table's width
# https://tex.stackexchange.com/questions/209574/problems-with-rowcolors-and-booktabs
%.pdf: %.tex
	sed -i 's/@{}/@{\\\\kern\\\\tabcolsep}/g' $$<
	sed -i 's/\\\\real/\\\\real{0.95} * \\\\real/g' $$<
	latexmk -xelatex -quiet -interaction=nonstopmode -synctex=1 -file-line-error $$<

# Rule: create LaTeX document
%.tex: %.md $(SETTINGS)
	pandoc -d defaults.yaml --template stenciler.latex $$< -o $$@

# Rule: create html document
%.html: %.md $(SETTINGS)
	pandoc -d defaults.yaml $$< -o $$@
endef

export submakefile