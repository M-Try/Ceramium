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

clean:
	@echo "Cleaning..."
	@rm $(BIN_DIR)/objs/*.o
	@rm $(BIN_DIR)/*.*
	@echo "Done."