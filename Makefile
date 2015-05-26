.PHONY: clean All

All:
	@echo "----------Building project:[ scrapies - Debug ]----------"
	@cd "scrapies" && $(MAKE) -f  "scrapies.mk"
clean:
	@echo "----------Cleaning project:[ scrapies - Debug ]----------"
	@cd "scrapies" && $(MAKE) -f  "scrapies.mk" clean
