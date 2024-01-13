//=============================================================================
//
// パーティクル処理 [particle.h]
// Author : 相馬靜雅
//
//=============================================================================

#ifndef _PARTICLE_H_		//このマクロ定義がされていなかったら
#define _PARTICLE_H_		//二重インクルード防止のマクロを定義する

//==========================================================================
// 名前空間
//==========================================================================
// 定数定義
namespace my_particle
{
	// 列挙型定義
	typedef enum
	{
		TYPE_NONE = 0,		// 煙のパーティクル
		TYPE_SMOKE,			// 煙のパーティクル
		TYPE_SMOKE_RED,		// 煙のパーティクル赤
		TYPE_SMOKE_YEL,		// 煙のパーティクル黄
		TYPE_EXPLOSION,		// 爆発のパーティクル
		TYPE_MANYTXT_MINI,	// 大量発生のUI(ミニ)
		TYPE_RESULTMESSAGE,	// リザルトメッセージ
		TYPE_OFFSETTING,	// 弾打ち消し
		TYPE_SUPERATTACK,	// 超攻撃
		TYPE_ICE_SPAWN,		// 氷出現
		TYPE_CHECK,			// 確認用
		TYPE_CHECK2D,			// 確認用
		TYPE_SICKLELINE,	// 大鎌のライン
		TYPE_MAGIC_EXPLOSION,	// 魔法の爆発
		TYPE_EXPLOSION_REMAIN,	// 魔法の爆発の残り
		TYPE_EXPLOSION_REMAIN_F,	// 魔法の爆発の残り,火
		TYPE_EXPLOSION_START,	// 魔法の爆発開始
		TYPE_WAVESMOKE,		// 波の煙
		TYPE_IMPACTWAVESMOKE,		// 円形波の煙
		TYPE_ENEMYSPAWN,	// 敵スポーン
		TYPE_NATURALWAVE_SPAWN,	// 自然の波
		TYPE_ENEMY_FADE,	// 敵のフェード
		TYPE_WATERIN,	// 水入った時
		TYPE_MOVESMOKE,	// 移動の煙
		TYPE_ENEMYKILL_COMBO,	// コンボ時の敵倒した
		TYPE_ADDSCORE,			// スコア加算時
		TYPE_BRASTATTACK,		// ブラストアタック
		TYPE_APPEARANCE_UNION,		// 合体の出現時
		TYPE_APPEARANCE_ARMTOARM,	// 腕×腕合体の出現
		TYPE_ATTACK_BODY,			// 胴攻撃
		TYPE_BEAMHIT_FIELD,			// ビームヒット(地面)
		TYPE_UNDERBOSS_SPAWN,		// 中ボススポーン
		TYPE_EVOLUSION_DECIDE,		// 進化完了
		TYPE_BEAM_CHARGE,		// ビームチャージ中
		TYPE_MORTAR_CHARGE,		// 迫撃チャージ中
		TYPE_UNIONWALK,			// 合体歩き
		TYPE_ULT_BEAM_CHARGE,		// 必殺ビームチャージ中
		TYPE_FALLSNOW,		// 降る雪
		TYPE_MAX
	}TYPE;

	void Create(MyLib::Vector3 pos, my_particle::TYPE nType);
}

#endif