# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

NetherCrown is a multiplayer action game built on Unreal Engine 5.7. It uses a component-based architecture with C++ for core systems and Blueprints for data configuration.

## Build Commands

```bash
# Build from command line (requires UE5.7 installed)
"C:\Program Files\Epic Games\UE_5.7\Engine\Build\BatchFiles\Build.bat" NetherCrown Win64 Development "E:\WorkSpace\NetherCrown\NetherCrown.uproject" -waitmutex

# Generate Visual Studio project files
"C:\Program Files\Epic Games\UE_5.7\Engine\Build\BatchFiles\GenerateProjectFiles.bat" "E:\WorkSpace\NetherCrown\NetherCrown.uproject"

# Open in editor
"C:\Program Files\Epic Games\UE_5.7\Engine\Binaries\Win64\UnrealEditor.exe" "E:\WorkSpace\NetherCrown\NetherCrown.uproject"
```

For typical development, open `NetherCrown.sln` in Visual Studio/Rider and build from IDE.

## Architecture

### Core Systems

The game uses a **component-aggregation pattern** on the player character (`ANetherCrownCharacter`):
- `UNetherCrownSkillComponent` - Manages skill objects and activation
- `UNetherCrownBasicAttackComponent` - Combo-based melee attacks with auto-targeting
- `UNetherCrownEquipComponent` - Weapon equip/stow mechanics
- `UNetherCrownCrowdControlComponent` - Status effects (knockback, frozen)
- `UNetherCrownPlayerStatComponent` (on PlayerState) - Character stats

### Skill System

Skills inherit from `UNetherCrownSkillObject` (UObject with network replication):
- `QSkill` - SkyFallSlash (dash attack with arc detection)
- `ESkill` - ShieldMastery (spawns shield actor)
- `RSkill` - FrozenTempest (AoE freeze with post-process effects)
- `Shift` - DashAttack (velocity-based movement)

**To add a new skill:**
1. Create class inheriting from `UNetherCrownSkillObject`
2. Override `PlaySkillCosmetics()` for animations/VFX
3. Override `ExecuteSkillGameplay()` for damage/effects
4. Add to `SkillObjectClasses` array in Blueprint
5. Add animation notify bindings in `UNetherCrownKnightAnimInstance`

### Networking Pattern

Server-authoritative with cosmetic multicasting:
- **Server RPCs** (`Server_*`) for gameplay: damage, skill activation, equipment
- **Multicast RPCs** (`Multicast_*`) for cosmetics: animations, sounds, VFX
- Skills replicate as UObject subobjects via `ReplicateSubobjects()`

### Data-Driven Configuration

- `UNetherCrownDefaultSettings` (DeveloperSettings) - Global config accessible in Project Settings
- `UNetherCrownWeaponData` (DataAsset) - Weapon stats
- DataTables for sounds/effects, looked up via GameplayTags through `FNetherCrownUtilManager`

### Animation Integration

Gameplay states are driven by animation notifies in `UNetherCrownCharacterAnimInstance`:
- Combo windows, hit trace enable/disable, equipment attach points
- This decouples animation timing from code

### Input System

Uses Enhanced Input System. Input Actions are in `Content/Input/InputActions/`. Controller (`ANetherCrownPlayerController`) calls character methods, which delegate to components.

## Key Source Directories

```
Source/NetherCrown/
├── Character/       # Player character, controller, anim instances
├── Components/      # Core gameplay components
├── Skill/           # Skill implementations
├── Enemy/           # Enemy character and animations
├── Weapon/          # Weapon actor and trace detection
├── Data/            # DataAssets (WeaponData, SoundData, EffectData)
├── Settings/        # DeveloperSettings for global config
├── Tags/            # GameplayTag declarations
└── Util/            # FNetherCrownUtilManager for tag-based lookups
```

## Collision

Custom collision channel `ECC_Enemy` defined in DefaultEngine.ini for enemy detection. Skills use sphere/arc detection; weapons use line traces via `UNetherCrownWeaponTraceComponent`.

## Module Dependencies

```
Core, CoreUObject, Engine, InputCore, EnhancedInput, UMG,
DeveloperSettings, GameplayTags, Niagara
```
