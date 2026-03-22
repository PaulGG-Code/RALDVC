.PHONY: build run shell clean rebuild

# Build the Docker image
build:
	docker compose build

# Start an interactive shell in the lab container
run:
	docker compose run --rm c-vuln-lab

# Alias for run
shell: run

# Remove the built image and containers
clean:
	docker compose down --rmi local --volumes --remove-orphans

# Rebuild from scratch (no cache)
rebuild:
	docker compose build --no-cache

# Build all vulnerability modules locally (requires gcc on host)
build-local:
	@for dir in vulnerabilities/*/; do \
		echo "=== Building $$dir ==="; \
		$(MAKE) -C "$$dir" all 2>&1 || echo "FAILED: $$dir"; \
	done

help:
	@echo ""
	@echo "C Vulnerability Learning Lab"
	@echo "============================"
	@echo ""
	@echo "  make build        - Build the Docker image"
	@echo "  make run          - Start an interactive lab shell (Docker)"
	@echo "  make rebuild      - Rebuild Docker image from scratch"
	@echo "  make clean        - Remove containers and images"
	@echo ""
	@echo "Inside the container, navigate to a module and run:"
	@echo "  cd /lab/vulnerabilities/01_stack_buffer_overflow"
	@echo "  make              # compile vulnerable + patched"
	@echo "  make demo         # run the exploit"
	@echo "  make demo-patched # run exploit against patched binary"
	@echo ""
