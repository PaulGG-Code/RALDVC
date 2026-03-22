# Top-level exercises Makefile
EXERCISES = exoA exoB exoC exoD exoE exoF exoG exoH

.PHONY: all clean demo $(EXERCISES)

all:
	@for ex in $(EXERCISES); do \
		echo "=== Building $$ex ==="; \
		$(MAKE) -C $$ex all 2>&1 || echo "FAILED: $$ex"; \
	done

demo:
	@for ex in $(EXERCISES); do \
		echo ""; \
		echo "══════════════════════════════════════════"; \
		echo "  Running $$ex"; \
		echo "══════════════════════════════════════════"; \
		$(MAKE) -C $$ex demo 2>&1 || true; \
	done

clean:
	@for ex in $(EXERCISES); do \
		$(MAKE) -C $$ex clean 2>/dev/null || true; \
	done
