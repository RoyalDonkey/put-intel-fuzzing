MAIN_TEX := main.tex
BUILD_DIR := build
OUT := dokumentacja

all: clean
	mkdir -p -- $(BUILD_DIR)
	@echo
	@echo '*** COMPILING [1/3] ***'
	xelatex -interaction batchmode -8bit -shell-escape -output-directory $(BUILD_DIR) -jobname $(OUT) -- $(MAIN_TEX) || true
	@echo
	@echo '*** COMPILING [2/3] ***'
	xelatex -interaction batchmode -8bit -shell-escape -output-directory $(BUILD_DIR) -jobname $(OUT) -- $(MAIN_TEX) || true
	@echo
	@echo '*** COMPILING [3/3] ***'
	xelatex -interaction batchmode -8bit -shell-escape -output-directory $(BUILD_DIR) -jobname $(OUT) -- $(MAIN_TEX) || true
	cp -- $(BUILD_DIR)/$(OUT).pdf .
	@echo
	@echo '*** BUILD FINISHED ***'

clean:
	$(RM) -r -- $(OUT).pdf $(BUILD_DIR)
