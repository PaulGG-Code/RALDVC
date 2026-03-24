MODULES = \
	01_stack_buffer_overflow \
	02_heap_buffer_overflow \
	03_format_string \
	04_integer_overflow \
	05_use_after_free \
	06_double_free \
	07_null_pointer_deref \
	08_off_by_one \
	09_command_injection \
	10_race_condition_toctou

.PHONY: all demo demo-patched clean $(MODULES)

# Build all modules
all:
	@for m in $(MODULES); do \
		echo "=== Building $$m ==="; \
		$(MAKE) -C $$m all 2>&1 || echo "FAILED: $$m"; \
	done

# Run the exploit demo for every module
demo:
	@for m in $(MODULES); do \
		echo ""; \
		echo "========================================"; \
		echo "  Module: $$m"; \
		echo "========================================"; \
		$(MAKE) -C $$m demo 2>&1 || echo "FAILED: $$m"; \
	done

# Run the patched demo for every module
demo-patched:
	@for m in $(MODULES); do \
		echo ""; \
		echo "========================================"; \
		echo "  Module: $$m (patched)"; \
		echo "========================================"; \
		$(MAKE) -C $$m demo-patched 2>&1 || echo "FAILED: $$m"; \
	done

clean:
	@for m in $(MODULES); do \
		$(MAKE) -C $$m clean 2>&1; \
	done
