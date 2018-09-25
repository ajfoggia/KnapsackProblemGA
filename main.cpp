//Alex Foggia
//This program uses a genetic algorithm to find near optimal solutions to the knapsack problem.
//All chromosomes are randomly generated for the first generation and are represented by a binary string.
//A 1 in the string signifies that item is included in the knapsack. While a 0 means the item is excluded.
//The items values and weights are also randomly generated, but remain constant after that for the duration of the program.
//For the selection process: Tournament selection.
//For the crossover process: a random crossover point is picked in the chromosome and then crossover is performed.
//For the mutation process: There is a 5% chance of mutation in the chromosome, if mutation is done then the bit in the
//string is flipped.

#include <iostream>
#include <cstring>
#include <string>
#include <cstdlib>
#include <ctime>
#include <cmath>

using std::string;

#define POP_SIZE 10  //Max population size.
#define CHROMOSOME_LENGTH 10  //Max chromosome length.
#define MAX_GENERATIONS 2  //Max number of generations.
#define KNAPSACK_WEIGHT_CAP 25  //The weight capacity of the knapsack.

//This is the structure of our chromosome.
struct chromosome
{
    string chrom_bits;  //Holds the binary string of item selections for the candidate solution.

    int fitness;  //Holds the fitness of the current chromosome.

    chromosome(): chrom_bits(""), fitness(0){};  //This initializes a chromosome.
};

///////////////////////////////////////////PROTOTYPES///////////////////////////////////////

string tournamentSelection(chromosome* Population);
string Crossover(string &firstParent, string &secondParent);
int calculateChromWeight(string &chrom_bits, int Weight[]);
int calculateFitness(string &chrom_bits, int Value[]);
void Mutate(string &chrom_bits);
void generateChromosomes(chromosome* Population, int count);
void generateItems(int Value[], int Weight[]);
void printPopulation(chromosome* Population, int generation);

//////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////MAIN/////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

int main()
{
    chromosome temp_pop[POP_SIZE];  //Temporary population array that holds the new population as it is being created.
    chromosome Population[POP_SIZE];  //Our current population.
    int gen = 0;  //Keeps track of the current generation being generated.
    int values[CHROMOSOME_LENGTH];  //Array that holds the values of each item.
    int weights[CHROMOSOME_LENGTH];  //Array that holds the weights of each item.
    int chrom_Weight = 0;  //Holds the weight of the current chromosome. Used to check if the chromosome has exceeded the weight constraint.

    srand((int)time(NULL));  //Seed the random number generator.

    //Create the first population with random chromosomes.
    for (int i = 0; i < POP_SIZE; i++)
    {
        generateChromosomes(Population, i);
    }

    //Generate the values and weights for the items.
    generateItems(values, weights);

    //Figure out the fitness of each random chromosome now that we have the items weights and values.
    for (int i = 0; i < POP_SIZE; i++)
    {
        //First we have to figure out if our chromosome is valid.
        chrom_Weight = calculateChromWeight(Population[i].chrom_bits, weights);
        //As long as the chromosome violates the knapsack weight cap, throw it out and generate a new one.
        while (chrom_Weight > KNAPSACK_WEIGHT_CAP)
        {
            std::cout << "\nCHROMOSOME " << i << " MUST BE EXCLUDED. IT EXCEEDS WEIGHT CAP!\n";
            std::cout << "\nWeight was: " << chrom_Weight << "\n";
            Population[i].chrom_bits = "";
            generateChromosomes(Population, i);
            std::cout << "\nCHROMOSOME " << i << " was changed to: " << Population[i].chrom_bits << "\n";
            chrom_Weight = calculateChromWeight(Population[i].chrom_bits, weights);
        }
        //Now we can calculate how good our valid chromosome is.
        Population[i].fitness = calculateFitness(Population[i].chrom_bits, values);
    }

    //Print the first generation.
    printPopulation(Population, gen);

    //While the number of generations has not been reached, create new generations.
    for (int i = 0; i < MAX_GENERATIONS; i++)
    {
        gen++;
        for (int j = 0; j < POP_SIZE; j++)
        {
            //We have the initial population created. Now we use tournament selection to select two parents.

            std::cout << "\nPERFORMING TOURNAMENT!";
            //Grab the first parent winner.
            string firstParent = tournamentSelection(Population);
            //Calculate the fitness of the first parent.
            int firstParentFitness = calculateFitness(firstParent, values);
            std::cout << "\nFIRST TOURNAMENT!";
            std::cout << "\nFirst tournament winner: " << firstParent << " with a fitness of " << firstParentFitness;

            //Now grab the second parent winner.
            string secondParent = tournamentSelection(Population);
            //Calculate the fitness of the second parent.
            int secondParentFitness = calculateFitness(secondParent, values);
            std::cout << "\nSECOND TOURNAMENT!";
            std::cout << "\n\nSecond tournament winner: " << secondParent << " with a fitness of " << secondParentFitness;

            //Now select the best of the two parents based on fitness to carry over into the next generation.
            //If the first parent has a better fitness, we want that parent.
            if (firstParentFitness > secondParentFitness)
            {
                temp_pop[j].chrom_bits = firstParent;
                temp_pop[j].fitness = firstParentFitness;
                std::cout << "\nTemporary population chrom " << j << ": " << temp_pop[j].chrom_bits << "\nFitness: " << temp_pop[j].fitness;
            }
            //Else if the second parent has a better fitness, then we want that parent.
            else if (firstParentFitness < secondParentFitness)
            {
                temp_pop[j].chrom_bits = secondParent;
                temp_pop[j].fitness = secondParentFitness;
                std::cout << "\nTemporary population chrom " << j << ": " << temp_pop[j].chrom_bits << "\nFitness: " << temp_pop[j].fitness;
            }
            //If they have the same fitness, we will just grab the first parent as default.
            else
            {
                temp_pop[j].chrom_bits = firstParent;
                temp_pop[j].fitness = firstParentFitness;
                std::cout << "\nTemporary population chrom " << j << ": " << temp_pop[j].chrom_bits << "\nFitness: " << temp_pop[j].fitness;
            }

            std::cout << "\n\n\nNOW PERFORMING CROSSOVER!\n";
            //Perform crossover on parents to yield one new offspring for the next generation.
            string Offspring = Crossover(firstParent, secondParent);

            std::cout << "\n\n\nNOW PERFORMING MUTATION!\n";
            //Then mutate the offspring (dependent on mutation rate.)
            Mutate(Offspring);
            std::cout << "Mutated first offspring: " << Offspring;
            //Calculate the fitness of the offspring.
            int offspringFitness = calculateFitness(Offspring, values);

            j++;
            //Now add the offspring to the new population.
            temp_pop[j].chrom_bits = Offspring;
            temp_pop[j].fitness = offspringFitness;
            std::cout << "\nTemporary population chrom " << j << ": " << temp_pop[j].chrom_bits << "\nFitness: " << temp_pop[j].fitness << "\n";
        }

        //Move the temporary population into the main population.
        for (int popCount = 0; popCount < POP_SIZE; popCount++)
        {
            Population[popCount].chrom_bits = temp_pop[popCount].chrom_bits;
            Population[popCount].fitness = temp_pop[popCount].fitness;
        }

        printPopulation(Population, gen);
    }

    std::cout << "\n\n\n";

    return 0;
}

//This function performs the crossover on the offspring selected by the tournament.
//The random crossover point is dependent on the crossover rate.
string Crossover(string &firstParent, string &secondParent)
{
    //Create the random crossover point.
    int crossover = (rand()% CHROMOSOME_LENGTH);
    std::cout << "\nRandom crossover point picked is: " << crossover;

    //Generate the new offspring from the crossovers of the parents.
    string tempChromosome = firstParent.substr(0, crossover) + secondParent.substr(crossover, CHROMOSOME_LENGTH);
    std::cout << "\nOffspring after crossover: " << tempChromosome;

    return tempChromosome;
}

//This function performs a tournament selection on the current population.
//It grabs two chromosomes at random to be put into a tournament.
//The winner (the one with the higher fitness) goes on to the next generation.
string tournamentSelection(chromosome* Population)
{
    //Select two chromosomes from the population at random and perform the tournament.

    //Preserve the random choices for the chromosomes so we know which string to return.
    int firstIndexChoice = (rand()% POP_SIZE);
    int secondIndexChoice = (rand()% POP_SIZE);

    std::cout << "\nFirst choice in the tournament is: " << Population[firstIndexChoice].chrom_bits;
    std::cout << " with a fitness of " << Population[firstIndexChoice].fitness;
    std::cout << "\nSecond choice in the tournament is: " << Population[secondIndexChoice].chrom_bits;
    std::cout << " with a fitness of " << Population[secondIndexChoice].fitness;


    //If the fitness of the first chromosome is greater than the second, give us that one
    //for the next population.
    if (Population[firstIndexChoice].fitness > Population[secondIndexChoice].fitness)
    {
        return Population[firstIndexChoice].chrom_bits;
    }
    //Otherwise, the second chromosome won and we want that one.
    else
    {
        return Population[secondIndexChoice].chrom_bits;
    }
}

//This function calculates the weight of a chromosome.
int calculateChromWeight(string &chrom_bits, int Weight[])
{
    int currentWeight = 0;  //Keeps track of the current weight of the chromosome to compare with the knapsack cap.

    for (int count = 0; count < CHROMOSOME_LENGTH; count++)
    {
        if (chrom_bits.at(count) == '1')
        {
            currentWeight += Weight[count];
        }
        else
        {
            //Do nothing because the item is not added.
        }
    }

    return currentWeight;
}

//This function calculates the fitness of a chromosome.
int calculateFitness(string &chrom_bits, int Value[])
{
    int currentValue = 0;  //Keeps track of the current value of the chromosome.

    for (int count = 0; count < CHROMOSOME_LENGTH; count++)
    {
        if (chrom_bits.at(count) == '1')
        {
            currentValue += Value[count];
        }
        else
        {
            //Do nothing because the item is not added.
        }
    }

    return currentValue;
}

//This function mutates a chromosome (dependent on the mutation rate).
void Mutate(string &chrom_bits)
{
    //For however long a chromosome is, mutate the bit if the 5% mutation rate is satisfied.
    for (int i = 0; i < CHROMOSOME_LENGTH; i++)
    {
        if (rand()% 100 < 5)
        {
            if (chrom_bits[i] == '0')
            {
                chrom_bits[i] = '1';
            }
            else
            {
                chrom_bits[i] = '0';
            }
        }
    }
}

//This function randomly generates the first chromosome strings.
void generateChromosomes(chromosome* Population, int count)
{
    for (int j = 0; j < CHROMOSOME_LENGTH; j++)
    {
        if (rand()% 2 == 0)
        {
            Population[count].chrom_bits += "0";
        }
        else
        {
            Population[count].chrom_bits += "1";
        }
    }
}

//This function randomly generates the items with values and weights.
void generateItems(int Value[], int Weight[])
{
    for (int j = 0; j < CHROMOSOME_LENGTH; j++)
    {
        //Give the item a random value.
        Value[j] = (rand()% 10);

        //Give the item a random weight.
        Weight[j] = (rand()% 10);
    }
}

//This function prints out the population of each generation.
void printPopulation(chromosome* Population, int generation)
{
    std::cout << "\n\nGENERATION " << generation << "\n";
    std::cout << "------------------------\n";
    for (int i = 0; i < POP_SIZE; i++)
    {
        std::cout << "Chromosome " << i << ": " << Population[i].chrom_bits << "\nFitness: " << Population[i].fitness << "\n";
    }
}