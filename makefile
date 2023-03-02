CXX := g++
BIN_DIR := ./bin
LIB_STATIC := ceramium.a
LIB_DYNAMIC := libceramium.so.1


compile_objs:
	@echo "Compiling object files..."
	@$(CXX) -c -fPIC ./src/*.cpp ./src/exceptions/*.cpp
	@mv *.o $(BIN_DIR)/objs
	@echo "Compiled successfully"

compile_lib_static: compile_objs
	@echo "Creating static library:"
	@echo "Packaging..."
	@ar -cvq $(BIN_DIR)/$(LIB_STATIC) $(BIN_DIR)/objs/*.o
	@echo "Done."

compile_lib_dynamic: compile_objs
	@echo "Creating dynamic library:"
	@echo "Making shared object..."
	@$(CXX) -shared $(BIN_DIR)/objs/*.o -o $(BIN_DIR)/$(LIB_DYNAMIC)
	@echo "Done."

test_static: compile_lib_static
	@echo "Compiling testcera.cpp with static library"
	@$(CXX) ./test/testcera.cpp $(BIN_DIR)/$(LIB_STATIC) -o $(BIN_DIR)/test_st.ELF
	@echo "Compiled successfully. Running test..."
	$(BIN_DIR)/test_st.ELF
	@echo "Done."

test_dynamic: compile_lib_dynamic
	@echo "Compiling testcera.cpp with dynamically linked library"
	@$(CXX) ./test/testcera.cpp -o $(BIN_DIR)/test_dy.ELF -l$(LIB_DYNAMIC) -L$(BIN_DIR)/
	@echo "Compiled successfully. Running test..."
	$(BIN_DIR)/test_dy.ELF
	@echo "Done."

clean:
	@echo "Cleaning..."
	-@rm $(BIN_DIR)/objs/*.o
	-@rm $(BIN_DIR)/*.*
	@echo "Done."