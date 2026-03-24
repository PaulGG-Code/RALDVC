# Top-level exercises Makefile
EXERCISES = exoA exoB exoC exoD exoE exoF exoG exoH exoI exoJ

.PHONY: all clean demo check $(EXERCISES)

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

check: all
	@PASS=0; FAIL=0; \
	for ex in $(EXERCISES); do \
		echo ""; \
		echo "── $$ex ──"; \
		if $(MAKE) -C $$ex check 2>&1; then \
			PASS=$$((PASS+1)); \
		else \
			FAIL=$$((FAIL+1)); \
			echo "[FAIL] $$ex a échoué."; \
		fi; \
	done; \
	echo ""; \
	echo "══════════════════════════════════════════"; \
	echo "  Résultat : $$PASS passés, $$FAIL échoués"; \
	echo "══════════════════════════════════════════"; \
	test $$FAIL -eq 0

clean:
	@for ex in $(EXERCISES); do \
		$(MAKE) -C $$ex clean 2>/dev/null || true; \
	done
