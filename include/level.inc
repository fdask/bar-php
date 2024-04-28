<?php
class Level {
    public $level;
    public $levelTicks;
    public $levelColor;

    public $targetCount;
    public $targetSize;
    public $targetProgress;
    
    public $targetTicks;
    public $targetColor;

    public $minBarSpeed;
    public $maxBarSpeed;
    public $shotCapacity;
    public $rocks;

    public function __construct() {
        $this->level = 1;
        $this->targetCount = Config::$targetCountStart;
        $this->targetSize = Config::$targetSizeStart;
        $this->targetProgress = 0;
        $this->minBarSpeed = Config::$minBarSpeed;
        $this->maxBarSpeed = Config::$maxBarSpeed;
        $this->shotCapacity = Config::$defaultShots;
        $this->rocks = false;
    }

    public function levelUp() {
        $this->level++;
        $this->levelTicks = Config::$statColorDuration;
        $this->levelColor = 'green';

        $this->targetProgress = 0;
        $this->rocks = false;

        // random bonus (or penalty) for reaching the next level
        switch (rand(0, 8)) {
            case 0:
                // add an extra target
                $this->targetCount++;

                break;
            case 1:
                // shrink target size
                if ($this->targetSize > 1) {
                    $this->targetSize--;
                }

                break;
            case 2:
                // enable rocks
                $this->rocks = true;

                break;
            case 3:
                // lower minimum
                if ($this->minBarSpeed - 25 > 0) {
                    $this->minBarSpeed = $this->minBarSpeed - 25;
                }

                break;
            case 4:
                // higher minimum 
                if ($this->minBarSpeed + 50 < $this->maxBarSpeed) {
                    $this->minBarSpeed = $this->minBarSpeed + 50;
                }

                break;
            case 5:
                // default shots up 1
                $this->shotCapacity++;

                break;
            case 6:
                // shot capacity down one
                $this->shotCapacity--;

                break;
            case 7:
                // lower maximum
                if ($this->maxBarSpeed - 25 > $this->minBarSpeed) {
                    $this->maxBarSpeed = $this->maxBarSpeed - 25;
                }

                break;
            case 8:
                // higher maxmium
                $this->maxBarSpeed = $this->maxBarSpeed + 50;

                break;
            default:
                // shouldn't be here
        }
    }

    public function hit() {
        $this->targetProgress++;
        $this->targetTicks = Config::$statColorDuration;
        $this->targetColor = 'green';
    }

    public function tick() {
        if ($this->targetTicks > 0) {
            $this->targetTicks--;
        }

        if ($this->levelTicks > 0) {
            $this->levelTicks--;
        }
    }
}