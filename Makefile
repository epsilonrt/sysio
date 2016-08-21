.PHONY: clean All

All:
	@echo "----------Building project:[ util_bsend -  ]----------"
	@cd "util/blyss/bsend" && "$(MAKE)" -f  "util_bsend.mk"
clean:
	@echo "----------Cleaning project:[ util_bsend -  ]----------"
	@cd "util/blyss/bsend" && "$(MAKE)" -f  "util_bsend.mk" clean
