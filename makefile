CXX := g++
CXX_FLAGS := -O2
BIN_DIR := ./bin
LIB_STATIC := ceramium.a
LIB_DYNAMIC := libceramium.so.1

default: compile_libs_all clean_objs

compile_libs_all: compile_lib_static compile_lib_dynamic

compile_objs:
	@echo "Compiling object files..."
	@$(CXX) $(CXX_FLAGS) -c -fPIC ./src/*.cpp ./src/exceptions/*.cpp
	@mv *.o $(BIN_DIR)/objs

compile_lib_static: compile_objs
	@echo "Creating static library:"
	@echo "Packaging..."
	@ar -cq $(BIN_DIR)/$(LIB_STATIC) $(BIN_DIR)/objs/*.o
	@echo "Done."

compile_lib_dynamic: compile_objs
	@echo "Creating dynamic library:"
	@echo "Making shared object..."
	@$(CXX) -fPIC -shared $(BIN_DIR)/objs/*.o -o $(BIN_DIR)/$(LIB_DYNAMIC)
	@echo "Done."

test_static: compile_lib_static
	@echo "Compiling testcera.cpp with static library"
	@$(CXX) $(CXX_FLAGS) ./test/testcera.cpp -o $(BIN_DIR)/test_st.ELF -l:$(LIB_STATIC) -L$(BIN_DIR)/ -I. -g
	@echo "Compiled successfully. Running test..."
	$(BIN_DIR)/test_st.ELF
	@echo "Done."

test_dynamic: compile_lib_dynamic
	@echo "Compiling testcera.cpp with dynamically linked library"
	@$(CXX) $(CXX_FLAGS) ./test/testcera.cpp -o $(BIN_DIR)/test_dy.ELF -L$(BIN_DIR)/ -lceramium -I.
	@echo "Compiled successfully. Running test..."
	@$(BIN_DIR)/test_dy.ELF
	@echo "Done."

clean_objs:
	-@rm $(BIN_DIR)/objs/*.o
	@echo "Cleaned up object files."

clean_bin:
	-@rm $(BIN_DIR)/*.*
	@echo "Cleaned binaries."

clean: clean_objs clean_bin
	@echo "Cleaned all."
