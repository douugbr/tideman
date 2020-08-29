#include <cs50.h>
#include <stdio.h>
#include <string.h>

// Max number of candidates
#define MAX 9

// preferences[i][j] is number of voters who prefer i over j
int preferences[MAX][MAX];

// locked[i][j] means i is locked in over j
bool locked[MAX][MAX];

// Each pair has a winner, loser
typedef struct
{
    int winner;
    int loser;
}
pair;

// Array of candidates
string candidates[MAX];
pair pairs[MAX * (MAX - 1) / 2];

int pair_count;
int candidate_count;

// Function prototypes
bool vote(int rank, string name, int ranks[]);
void record_preferences(int ranks[]);
void add_pairs(void);
void sort_pairs(void);
void lock_pairs(void);
void print_winner(void);
bool create_cycle(int initial_candidate, int initial_winner);

int main(int argc, string argv[])
{
    // Check for invalid usage
    if (argc < 2)
    {
        printf("Usage: tideman [candidate ...]\n");
        return 1;
    }

    // Populate array of candidates
    candidate_count = argc - 1;
    if (candidate_count > MAX)
    {
        printf("Maximum number of candidates is %i\n", MAX);
        return 2;
    }
    for (int i = 0; i < candidate_count; i++)
    {
        candidates[i] = argv[i + 1];
    }

    // Clear graph of locked in pairs
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            locked[i][j] = false;
        }
    }

    pair_count = 0;
    int voter_count = get_int("Number of voters: ");

    // Query for votes
    for (int i = 0; i < voter_count; i++)
    {
        // ranks[i] is voter's ith preference
        int ranks[candidate_count];

        // Query for each rank
        for (int j = 0; j < candidate_count; j++)
        {
            string name = get_string("Rank %i: ", j + 1);

            if (!vote(j, name, ranks))
            {
                printf("Invalid vote.\n");
                return 3;
            }
        }

        record_preferences(ranks);

        printf("\n");
    }

    add_pairs();
    sort_pairs();
    lock_pairs();
    print_winner();
    return 0;
}

// Update ranks given a new vote
bool vote(int rank, string name, int ranks[])
{

    for (int i = 0; i < candidate_count; i++)
    {
        if (strcmp(name, candidates[i]) == 0)
        {
            ranks[rank] = i;
            return true;
        }
    }

    return false;
}

// Update preferences given one voter's ranks
void record_preferences(int ranks[])
{
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            if (j > i)
            {
                preferences[ranks[i]][ranks[j]]++;
            }
        }
    }
    return;
}

// Record pairs of candidates where one is preferred over the other
void add_pairs(void)
{
    int index = -1;

    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            if (preferences[i][j] > preferences[j][i])
            {
                index++;
                pairs[index].winner = i;
                pairs[index].loser = j;
                pair_count++;
            }
        }
    }

    return;
}



// Sort pairs in decreasing order by strength of victory
void sort_pairs(void)
{
    int strengths[pair_count];

    for (int i = 0; i < pair_count; i++)
    {
        strengths[i] = preferences[pairs[i].winner][pairs[i].loser];
    }

    pair pair_new;
    int strengths_new;

    // Selection sort by https://www.geeksforgeeks.org/c-program-to-sort-an-array-in-ascending-order/
    // I'm so focused on the problem in general that I have no willing left to
    // develop my own sorting algorithm, so sorry about that
    for (int i = 0; i < pair_count - 1; i++)
    {
        int max_idx = i;
        for (int j = i + 1; j < pair_count; j++)
        {
            if (strengths[j] > strengths[max_idx])
            {
                max_idx = j;
            }
        }

        strengths_new = strengths[max_idx];
        strengths[max_idx] = strengths[i];
        strengths[i] = strengths_new;

        pair_new = pairs[max_idx];
        pairs[max_idx] = pairs[i];
        pairs[i] = pair_new;

    }
    return;
}

// Lock pairs into the candidate graph in order, without creating cycles
void lock_pairs(void)
{
    for (int i = 0; i < pair_count; i++)
    {
        if (!create_cycle(pairs[i].loser, pairs[i].winner))
        {
            locked[pairs[i].winner][pairs[i].loser] = true;
        }
    }
    return;
}

// Print the winner of the election
void print_winner(void)
{
    int counting[candidate_count];
    bool has_tie = false;
    string winner;

    for (int i = 0; i < candidate_count; i++)
    {
        counting[i] = 0;
    }

    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            if (locked[j][i] == false)
            {
                counting[i]++;
            }
            if (counting[i] == candidate_count && counting[j] == candidate_count)
            {
                has_tie = true;
            }
        }
        if (counting[i] == candidate_count && has_tie == false)
        {
            printf("%s\n", candidates[i]);
        }
        else
        {
            printf("%s\n", candidates[pairs[0].winner]);
        }
    }
    return;
}

bool create_cycle(int initial_candidate, int initial_winner)
{
    if (initial_candidate == initial_winner)
    {
        return true;
    }

    for (int i = 0; i < candidate_count; i++)
    {
        if ((initial_winner == pairs[i].loser) && locked[pairs[i].winner][initial_winner])
        {
            if (create_cycle(initial_candidate, pairs[i].winner))
            {
                return true;
            }
        }
    }
    return false;
}
