INC_DIR = ./include
SRC_DIR = ./src

EXT_INC = -I./ext
EXT_LIB = -L./ext/opensift/lib

CXX  = g++ -g -std=c++11 -I$(INC_DIR) $(EXT_INC) $(EXT_LIB)
OBJS = objs/global.o objs/util.o objs/image.o objs/canny_edge.o objs/watershed_edge.o \
	   objs/stereo.o objs/filter.o objs/cost.o objs/combine.o objs/sift_cost.o \
	   objs/row_propagate.o objs/mrf.o objs/gco_graph.o objs/gco.o objs/maxprod_bp.o \
	   objs/sift_match.o

main: $(SRC_DIR)/main.cc $(OBJS)
	$(CXX) $^ `pkg-config --libs --cflags opencv` -lopensift

objs/global.o: $(SRC_DIR)/global.cc $(INC_DIR)/global.h
	$(CXX) -c $< -o $@

objs/util.o: $(SRC_DIR)/util.cc $(INC_DIR)/util.h
	$(CXX) -c $< -o $@

objs/image.o: $(SRC_DIR)/image.cc $(INC_DIR)/image.h
	$(CXX) -c $< -o $@

objs/canny_edge.o: $(SRC_DIR)/canny_edge.cc $(INC_DIR)/canny_edge.h
	$(CXX) -c $< -o $@

objs/watershed_edge.o: $(SRC_DIR)/watershed_edge.cc $(INC_DIR)/watershed_edge.h
	$(CXX) -c $< -o $@

objs/stereo.o: $(SRC_DIR)/stereo.cc $(INC_DIR)/stereo.h
	$(CXX) -c $< -o $@

objs/filter.o: $(SRC_DIR)/filter.cc $(INC_DIR)/filter.h
	$(CXX) -c $< -o $@

objs/cost.o: $(SRC_DIR)/cost.cc $(INC_DIR)/cost.h
	$(CXX) -c $< -o $@

objs/combine.o: $(SRC_DIR)/combine.cc $(INC_DIR)/combine.h
	$(CXX) -c $< -o $@

objs/sift_cost.o: $(SRC_DIR)/sift_cost.cc $(INC_DIR)/sift_cost.h
	$(CXX) -c $< -o $@

objs/row_propagate.o: $(SRC_DIR)/row_propagate.cc $(INC_DIR)/row_propagate.h
	$(CXX) -c $< -o $@

objs/mrf.o: $(SRC_DIR)/mrf.cc $(INC_DIR)/mrf.h
	$(CXX) -c $< -o $@

objs/gco_graph.o: $(SRC_DIR)/gco_graph.cc $(INC_DIR)/gco_graph.h
	$(CXX) -c $< -o $@

objs/gco.o: $(SRC_DIR)/gco.cc $(INC_DIR)/gco.h
	$(CXX) -c $< -o $@

objs/maxprod_bp.o: $(SRC_DIR)/maxprod_bp.cc $(INC_DIR)/maxprod_bp.h
	$(CXX) -c $< -o $@

objs/sift_match.o: $(SRC_DIR)/sift_match.cc $(INC_DIR)/sift_match.h
	$(CXX) -c $< `pkg-config --cflags opencv` -o $@