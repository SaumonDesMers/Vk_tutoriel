target = vulkan_test

CXX = clang++
CXXFLAGS = -std=c++17 -O2 -g# -Wall -Wextra -Werror
LDFLAGS = -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi

VPATH = src src/app
SRCS = main.cpp logger.cpp application.cpp window.cpp pipeline.cpp device.cpp \
	   swap_chain.cpp model.cpp renderer.cpp render_system.cpp camera.cpp \
	   keyboard_movement_controller.cpp game_object.cpp
INC_DIR = -I include -I glm -I libs

OBJ_DIR = obj
DEP_DIR = dep

#-------------------------------------------------------------

ifeq ($(NDEBUG), 1)
	CXXFLAGS += -DNDEBUG
endif

ifeq ($(SANITIZE), 1)
	VALGRIND = valgrind --tool=memcheck --leak-check=full --leak-resolution=high --track-origins=yes --show-reachable=yes --log-file=valgrind.log
endif


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
	$(VALGRIND) ./$(target)

clean :
	$(RM) $(OBJS) $(DEPS)

fclean : clean
	$(RM) $(target)

re : fclean
	@$(MAKE) all --no-print-directory

.PHONY : all clean fclean re