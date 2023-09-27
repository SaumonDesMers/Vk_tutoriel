target = vulkan_test

CXX = clang++
CXXFLAGS = -std=c++17 -O2# -Wall -Wextra -Werror
LDFLAGS = -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi

VPATH = src src/application
SRCS = main.cpp window.cpp cleanup.cpp debug.cpp
INC_DIR = -I include

OBJ_DIR = obj
DEP_DIR = dep

#-------------------------------------------------------------

OBJS = $(SRCS:%.cpp=$(OBJ_DIR)/%.o)
DEPS = $(SRCS:%.cpp=$(DEP_DIR)/%.d)

all : $(target)

$(target) : $(OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^

-include $(DEPS)

$(OBJ_DIR)/%.o : %.cpp
	@mkdir -p $(OBJ_DIR) $(DEP_DIR)
	$(CXX) $(CXXFLAGS) -o $@ -MMD -MF $(DEP_DIR)/$*.d $(INC_DIR) -c $<

test: all
	@./$(target)

clean :
	$(RM) -rf $(OBJ_DIR)/*.o $(DEP_DIR)/*.d

fclean : clean
	$(RM) -rf $(target)

re : fclean
	@$(MAKE) all --no-print-directory

.PHONY : all clean fclean re