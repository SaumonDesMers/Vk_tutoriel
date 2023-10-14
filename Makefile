target = vulkan_test

CXX = clang++
CXXFLAGS = -std=c++17 -O2 -g# -Wall -Wextra -Werror
LDFLAGS = -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi

ifeq ($(NDEBUG), 1)
	CXXFLAGS += -DNDEBUG
endif

ifeq ($(SANITIZE), 1)
	VALGRIND = valgrind --tool=memcheck --leak-check=full --leak-resolution=high --track-origins=yes --show-reachable=yes --log-file=valgrind.log
endif

VPATH = src src/application
SRCS = main.cpp window.cpp clean_up.cpp debug.cpp instance.cpp main_loop.cpp \
		physical_device.cpp logical_device.cpp swap_chain.cpp image_view.cpp \
		render_pass.cpp graphics_pipeline.cpp frame_buffer.cpp command.cpp \
		sync_objects.cpp draw.cpp vertex_buffer.cpp buffer.cpp index.cpp \
		descriptor.cpp uniform_buffer.cpp texture.cpp depth.cpp model_loading.cpp \
		utils.cpp
INC_DIR = -I include -I glm

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
	$(VALGRIND) ./$(target)

clean :
	$(RM) $(OBJS) $(DEPS)

fclean : clean
	$(RM) $(target)

re : fclean
	@$(MAKE) all --no-print-directory

.PHONY : all clean fclean re