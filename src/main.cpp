#include <Geode/Geode.hpp>
#include <Geode/modify/GJGarageLayer.hpp>
#include <Geode/modify/RewardsPage.hpp>

using namespace geode::prelude;

std::string getNextAdChest() {
	for (int i = 12; i <= 21; ++i) {
		std::string chestID = "00" + std::to_string(i);
		if (!GameStatsManager::get()->isSpecialChestUnlocked(chestID)) return chestID;
	}
	return "";
}

class $modify(RewardsPageButAds, RewardsPage) {

	void onAdChest(CCObject* s) {
		FMODAudioEngine::get()->playEffect("chestClick.ogg");
		auto nextChest = getNextAdChest();

		if (nextChest == "") {
			noMoreChests();
			return;
		}

		GJRewardItem* adReward = GameStatsManager::get()->unlockSpecialChest(nextChest);

		auto rwd = RewardUnlockLayer::create(2, nullptr);
		rwd->show();
		rwd->showCollectReward(adReward);
	}

	bool init() {
		if (!RewardsPage::init()) return false;
		bool allChests = (getNextAdChest() == "");

		if (allChests && Mod::get()->getSettingValue<bool>("hideWhenDone")) return true;

		auto winSize = CCDirector::sharedDirector()->getWinSize();

		const char* adFrame = "GJ_adChestBtn_001.png";
		auto adSprite = allChests ? CCSpriteGrayscale::createWithSpriteFrameName(adFrame) : CCSprite::createWithSpriteFrameName(adFrame);

		float adScale = 0.65f;
		adSprite->setScale(adScale);

		auto adButton = CCMenuItemSpriteExtra::create(adSprite, this, menu_selector(RewardsPageButAds::onAdChest));
		adButton->setPosition({ 168, -100 });
		adButton->setID("ad-chest-button"_spr);
		m_buttonMenu->addChild(adButton);

		if (allChests) {
			adSprite->setColor(ccColor3B({200, 200, 200}));
			return true;
		}

		adSprite->runAction(CCRepeatForever::create(
			CCSequence::create(
				CCEaseSineInOut::create(CCScaleTo::create(0.5, adScale * 1.1f)),
				CCEaseSineInOut::create(CCScaleTo::create(0.5, adScale)),
				nullptr
			)
        ));

		return true;
	}

	void noMoreChests() {
		auto dialogLines = CCArray::create();

		dialogLines->addObject(DialogObject::create(
			"Scratch",
			"I don\'t have any <co>chests</c> left<d010>.<d010>.<d010>.",
			12, 1.0f, false, ccWHITE
		));

		dialogLines->addObject(DialogObject::create(
			"Scratch",
			"You took <cr>EVERYTHING</c> I had!",
			14, 1.0f, false, ccWHITE
		));

		dialogLines->addObject(DialogObject::create(
			"Scratch",
			"I need to find <cy>new stuff</c>...",
			13, 1.0f, false, ccWHITE
		));

		DialogLayer* dialog = DialogLayer::createWithObjects(dialogLines, 2);
		dialog->updateChatPlacement(DialogChatPlacement::Center);
		dialog->animateInRandomSide();
		dialog->addToMainScene();
	}
};


class $modify(GarageButSupporter, GJGarageLayer) {
	struct Fields : DialogDelegate {
		GJGarageLayer* self = nullptr;
        void dialogClosed(DialogLayer* p0) override {
			GameManager* gm = GameManager::get();
			gm->reportAchievementWithID("geometry.ach.mdrate", 100, false);
			auto winSize = CCDirector::sharedDirector()->getWinSize();

			auto cur = CurrencyRewardLayer::create(0, 0, 0, 0, CurrencySpriteType::Icon, 0, CurrencySpriteType::Icon, 0, winSize / 2, static_cast<CurrencyRewardType>(0), 0, 1);
			self->addChild(cur, 1000);
			
			auto icon = GJItemIcon::createBrowserItem(UnlockType::Cube, 71);
			cur->createUnlockObject(icon, winSize / 2, 0.5f);

			self->selectTab(IconType::Cube);
        }
	};

	void showUnlockPopup(int id, UnlockType type) {
		if (id == 71 && type == UnlockType::Cube) {
			auto dialogLines = CCArray::create();

			dialogLines->addObject(DialogObject::create(
				"Potbor",
				"Yo, what up.",
				19, 1.0f, false, ccWHITE
			));

			dialogLines->addObject(DialogObject::create(
				"Potbor",
				"You're supposed to be on <cr>Meltdown</c> unlock this <cg>cube</c>...",
				24, 1.0f, false, ccWHITE
			));

			dialogLines->addObject(DialogObject::create(
				"Potbor",
				"If anyone asks, you got this legitimately.",
				20, 1.0f, false, ccWHITE
			));

			dialogLines->addObject(DialogObject::create(
				"Potbor",
				"<cl>Is it just me, or is it getting kind of hot in here?</c>",
				24, 0.5f, false, ccWHITE
			));

			DialogLayer* dialog = DialogLayer::createWithObjects(dialogLines, 2);
			dialog->updateChatPlacement(DialogChatPlacement::Center);

  			m_fields->self = this;
			dialog->m_delegate = m_fields.self();

			dialog->animateInRandomSide();
			dialog->addToMainScene();
			return;
		}

		return showUnlockPopup(id, type);
	}
};
