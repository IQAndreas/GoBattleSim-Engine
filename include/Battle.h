
#ifndef _BATTLE_H_
#define _BATTLE_H_

#include "Player.h"
#include "TimelineEvent.h"

#include <vector>

namespace GoBattleSim
{

constexpr Player_Index_t MAX_NUM_PLAYERS = 24;

struct PvEBattleOutcome
{
	int duration{0};
	bool win{false};
	int tdo{0};
	double tdo_percent{0.0};
	int num_deaths{0};
	std::vector<PokemonState> pokemon_stats;
	std::vector<TimelineEvent> battle_log;
};

class Battle
{
public:
	void add_player(const Player *);
	Player *get_player(Player_Index_t idx);
	void erase_players();
	void set_time_limit(unsigned);
	void set_weather(int);
	void set_background_dps(unsigned);
	void set_enable_log(bool);
	void init();
	void start();
	PvEBattleOutcome get_outcome(int);
	const std::vector<TimelineEvent> &get_log();

protected:
	struct PlayerState
	{
		Player player;
		unsigned short head_index;
		unsigned time_free;
		Action current_action;
		Action buffer_action;
	};

	void fetch_pokemon(Player &);
	void erase_pokemon();

	short search(const Pokemon *);
	Player_Index_t search_rival(Player_Index_t);

	void enqueue(TimelineEvent &&);
	TimelineEvent dequeue();

	void go();

	void handle_fainted_pokemon(Player_Index_t);

	// three possible actions when a Pokemon faints
	bool select_next_pokemon(PlayerState &);
	bool revive_current_party(PlayerState &);
	bool select_next_party(PlayerState &);

	bool is_defeated(int);
	bool is_end();

	void register_action(Player_Index_t player_idx, const Action &);

	void register_action_fast(Player_Index_t player_idx, const Action &);
	void register_action_charged(Player_Index_t player_idx, const Action &);
	void register_action_dodge(Player_Index_t player_idx, const Action &);
	void register_action_switch(Player_Index_t player_idx, const Action &);
	void register_action_wait(Player_Index_t player_idx, const Action &);

	inline StrategyInput generate_strat_input(Player_Index_t player_idx);

	void next(const TimelineEvent &);

	void handle_event_free(const TimelineEvent &);
	void handle_event_announce(const TimelineEvent &);
	void handle_event_attack(const TimelineEvent &); // Fast and Charged
	void handle_event_dodge(const TimelineEvent &);
	void handle_event_background_dps(const TimelineEvent &);
	void handle_event_enter(const TimelineEvent &);

	inline void append_log(const TimelineEvent &);
	void erase_log();

private:
	std::vector<TimelineEvent> m_event_queue;
	std::vector<TimelineEvent> m_event_history;

	PlayerState m_player_states[MAX_NUM_PLAYERS];
	Player_Index_t m_players_count{0};

	Pokemon *m_pokemon[MAX_NUM_PLAYERS * MAX_NUM_PARTIES * MAX_NUM_POKEMON];
	PokemonState m_pokemon_states[MAX_NUM_PLAYERS * MAX_NUM_PARTIES * MAX_NUM_POKEMON];
	unsigned short m_pokemon_count{0};

	bool m_enable_log{false};
	unsigned m_time_limit{0};
	unsigned m_time{0};
	int m_weather{-1};
	int m_defeated_team{-1};
	unsigned m_background_dps{0};
};

} // namespace GoBattleSim

#endif