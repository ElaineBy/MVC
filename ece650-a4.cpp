// defined std::unique_ptr
#include <memory>
// defines Var and Lit
#include "minisat/core/SolverTypes.h"
// defines Solver
#include "minisat/core/Solver.h"
#include <vector>
#include <sstream>
#include <iostream>


//Calculate the Combination(m ,n)
long Combination(int m,int n)
{
    long result = 1;
    int k;
    
    for(k = 1; k <= n; k++){
        result = (result * (m - n + k)) / k;
    }
    return result;
}

//Strip the symbols from inout
std::string StripInput(std::string line){
    
    std::string::iterator it;
    //leave all the numbers and clear other characters
    for ( it = line.begin(); it != line.end();) {
        if (*it == '<') {
            line.erase(it);
        }
        else if (*it == '>') {
            line.erase(it);
        }
        else if (*it == '{') {
            line.erase(it);
        }
        else if (*it == '}') {
            line.erase(it);
        }
        else if (*it == ' ') {
            line.erase(it);
        }
        else
            ++it;
    }
    
    return line;
}

//Get edges
unsigned InitialEdge(unsigned edge[],unsigned vertex_num, std::vector<unsigned> nums){
    unsigned count = 0;
    unsigned flag = 0;
    
    for (unsigned x : nums) {
        if (x >= vertex_num)
            flag++;
        else{
            edge[count] = x;
            count++;
        }
    }
    return flag;
}

//Use minisat to figure out the mini vertex cover of graph
void GenerateVertexCover(unsigned edge_num, unsigned vertex_num, unsigned sconstrain_num, unsigned tconstrain_num, unsigned edge[]){
    
    unsigned literals[100][100];
    unsigned mvc = 1;
    unsigned left = 1;
    unsigned right = vertex_num;
    unsigned vertex_cover[100];
    //int middle = 1;
    //std::unique_ptr<Minisat::Solver> solver(new Minisat::Solver());
    //Minisat::vec<Minisat::Lit> vars;
  
    
    while(left <= right){
    //for(unsigned k = 1; k <= vertex_num;k++){
        unsigned middle = (left + right)/2;
        std::unique_ptr<Minisat::Solver> solver(new Minisat::Solver());
        Minisat::vec<Minisat::Lit> vars;
        //solver.reset (new Minisat::Solver());
        
        unsigned number = 1;
        for(unsigned i = 0; i < vertex_num; i++){
            for(unsigned j = 0; j < middle; j++){
                literals[i][j] =number;
                number++;
            }
        }
        
        sconstrain_num = vertex_num*Combination(middle,2);
        tconstrain_num = middle * Combination(vertex_num,2);
        
        
        for(unsigned i = 0; i < number; i++){
            
            Minisat::Lit l = Minisat::mkLit(solver->newVar());
            vars.push(l);
            
        }
        
        // add clauses
        Minisat::vec<Minisat::Lit> first_constrain[middle];
        for(unsigned c = 0; c < middle; c ++){
            for(unsigned b = 0; b < vertex_num; b++){
                first_constrain[c].push(vars[literals[b][c]]);
            }
            solver->addClause(first_constrain[c]);
        }
        
        unsigned second_count = 0;
        Minisat::vec<Minisat::Lit> second_constrain[sconstrain_num];
        for(unsigned c = 0; c < vertex_num; c ++){
            for(unsigned b = 0; b < middle-1; b++){
                for(unsigned a = b+1 ; a < middle; a++){
                    second_constrain[second_count].push(~vars[literals[c][b]]);
                    second_constrain[second_count].push(~vars[literals[c][a]]);
                    solver->addClause(second_constrain[second_count]);
                    second_count++;
                }
            }
        }
        
        unsigned third_count = 0;
        Minisat::vec<Minisat::Lit> third_constrain[tconstrain_num];
        for(unsigned c = 0; c < middle; c ++){
            for(unsigned b = 0; b < vertex_num - 1; b++){
                for(unsigned a = b+1 ; a < vertex_num; a++){
                    third_constrain[third_count].push(~vars[literals[b][c]]);
                    third_constrain[third_count].push(~vars[literals[a][c]]);
                    solver->addClause(third_constrain[third_count]);
                    third_count++;
                }
            }
        }
        
        unsigned fourth_count = 0;
        Minisat::vec<Minisat::Lit> fourth_constrain[edge_num];
        for(unsigned b = 0; b < edge_num*2 - 1; ){
            for(unsigned a = 0 ; a < middle; a++){
                unsigned x = edge[b];
                unsigned y = edge[b+1];
                fourth_constrain[fourth_count].push(vars[literals[x][a]]);
                fourth_constrain[fourth_count].push(vars[literals[y][a]]);
            }
            solver->addClause(fourth_constrain[fourth_count]);
            fourth_count++;
            b += 2;
        }
        
        
        // solve
        bool res = solver->solve();
        
        if(res == 0){
            left = middle + 1;
            //the next line de-allocates existing solver and allocates a newone in its place.
            solver.reset (new Minisat::Solver());
            //continue;
        }
        else{
            mvc = middle;
            right = middle - 1;
            unsigned k = 0;
            for(unsigned b = 0; b < vertex_num; b++){
                for(unsigned c = 0; c < mvc; c++){
                    if(Minisat::toInt(solver->modelValue(vars[literals[b][c]])) == 1)
                        continue;
                    else{
                        vertex_cover[k] = b;
                        k +=1;
                    }
                }
            }
            solver.reset (new Minisat::Solver());
            //break;
        }
    }
    
        for(unsigned k= 0; k < mvc; k++){
                std::cout << vertex_cover[k] <<" ";
    }
    std::cout << std::endl;
    //solver.reset (new Minisat::Solver());

     //std::cout << mvc << std::endl;
    //}
}


int main(void) {
    
    unsigned vertex_num = 0;
    
    while (!std::cin.eof()) {
        
        std::string line;
        std::getline(std::cin, line);
        
        if (line[0] == 'V') {
            
            line.erase(0, 1);
            std::istringstream input(line);
            std::string num;
            
            while (!input.eof()) {
                input >> num;
                if (stoi(num) <= 0) {
                    std::cerr << "Error:Invalid number of vertex\n";
                    break;
                }
                else {
                    vertex_num = stoi(num);
                }
            }
        }
        
        else if (line[0] == 'E') {
            
            line.erase(0, 1);
            unsigned edge_num = 0;
            unsigned sconstrain_num = 0;
            unsigned tconstrain_num = 0;
            //unsigned literals[500][500];
            unsigned num;
            unsigned flag = 0;
            unsigned edge[100];
            
            line = StripInput(line);
            
            if (!line.empty()) {
                std::istringstream input(line);
                std::vector<unsigned> nums;
                
                while (!input.eof()) {
                    input >> num;
                    nums.push_back(num);
                    
                    if (input.eof())
                        break;
                    char separator;
                    input >> separator;
                }
                
                if (!nums.empty()) {
                    
                    flag = InitialEdge(edge,vertex_num,nums);
                    
                    edge_num = nums.size()/2;
                    
                }
                
            }
            /*
            if(edge_num == 0)
                continue;
            
            else if(flag > 0)
                std::cerr << "Error:Invalid vertex index\n";
            
            else{
                */
                GenerateVertexCover(edge_num, vertex_num, sconstrain_num, tconstrain_num, edge);
           // }
        }
    }
    // at this point the solver is ready. You must create new variable and new clauses
    return 0;
}
