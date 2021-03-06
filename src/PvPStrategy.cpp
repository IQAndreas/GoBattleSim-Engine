
#include "PvPStrategy.h"

#include "GameMaster.h"

namespace GoBattleSim
{

/**
 * PvP Basic Strategy
 * Use default cmove as soon as possible
 */
void pvp_basic_on_free(const PvPStrategyInput &si, Action *r_action)
{
	if (si.subject_energy + si.subject->cmove->energy >= 0)
	{
		r_action->type = ActionType::Charged;
	}
	else
	{
		r_action->type = ActionType::Fast;
	}
}

/**
 * PvP Advance Strategy
 * See docs
 */
void pvp_advance_on_free(const PvPStrategyInput &si, Action *r_action)
{
	int my_fmove_damage = calc_damage(si.subject, si.subject->get_fmove(0), si.enemy, GameMaster::get().fast_attack_bonus_multiplier);
	if (my_fmove_damage >= si.enemy_hp)
	{
		r_action->type = ActionType::Fast;
		return;
	}

	// Find the better move and the cheaper move.
	int better_move_idx = 0, cheaper_move_idx = 0;
	double higher_dpe = 0;
	int lower_energy_cost = -1;
	bool cheaper_move_ko = false;
	for (unsigned i = 0; i < si.subject->cmoves_count; ++i)
	{
		auto move = si.subject->get_cmove(i);
		int damage = calc_damage(si.subject, move, si.enemy, GameMaster::get().charged_attack_bonus_multiplier);
		double dpe = (double)damage / (-move->energy);
		if (dpe > higher_dpe)
		{
			higher_dpe = dpe;
			better_move_idx = i;
		}
		if (-move->energy < lower_energy_cost || (-move->energy == lower_energy_cost && move->effect.activation_chance > 0) || lower_energy_cost < 0)
		{
			lower_energy_cost = -move->energy;
			cheaper_move_idx = i;
			cheaper_move_ko = damage >= si.enemy_hp;
		}
	}

	if (si.subject_energy + si.subject->get_cmove(better_move_idx)->energy >= 0)
	{
		if (si.enemy_shields > 0)
		{
			r_action->type = ActionType::Charged;
			r_action->value = cheaper_move_idx;
		}
		else
		{
			r_action->type = ActionType::Charged;
			r_action->value = better_move_idx;
		}
	}
	else if (si.subject_energy + si.subject->get_cmove(cheaper_move_idx)->energy >= 0)
	{
		if (cheaper_move_ko || calc_damage(si.enemy, &si.enemy->fmove, si.subject, GameMaster::get().fast_attack_bonus_multiplier) >= si.subject_hp)
		{
			r_action->type = ActionType::Charged;
			r_action->value = cheaper_move_idx;
		}
		else
		{
			r_action->type = ActionType::Fast;
		}
	}
	else
	{
		r_action->type = ActionType::Fast;
	}
}

} // namespace GoBattleSim
