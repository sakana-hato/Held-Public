#pragma once
#include "BoneAttachment.h"
#include "Capsule.h"

/// <summary>
/// 刀 汎用の BoneAttachment を使ってプレイヤーのボーンに追従する。
/// </summary>
class Katana
{
public:

	void Init(int handle);

	void SetWaistFrame(int frame) { waistFrame = frame; }
	void SetHandFrame(int frame) { handFrame = frame; }

	/// <summary>
	///抜刀（手へ）／納刀（腰へ）
	///</summary>
	void Unsheathe() { drawn = true; }
	void Sheathe() { drawn = false; }
	bool IsDrawn() const { return drawn; }

	void Update(int ownerModel);

	void Draw()const;

	Capsule GetBladeCapsule() const;

	void SetHandOffset(const MATRIX& m) { handOffset = m; }

private:
	BoneAttachment attach;

	int waistFrame = -1;
	int handFrame = -1;
	bool drawn = false;

	MATRIX waistOffset = MGetIdent();   
	MATRIX handOffset  = MGetIdent();   

};
