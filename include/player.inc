<?php
class Player {
    public $hits;
    public $misses;
    public $distance;
    
    public $health;
    public $healthTicks;
    public $healthColor;

    public $score;
    public $scoreTicks;
    public $scoreColor;
    
    public $shots;
    public $shotTicks;
    public $shotColor;

    public function __construct() {
        $this->hits = 0;
        $this->misses = 0;
        $this->distance = 0;
        $this->health = Config::$defaultHealth;
        $this->score = 0;
        $this->shots = Config::$defaultShots;
    }

    public function addToScore($s) {
        $this->score += $s;
        $this->scoreTicks = 5;
        $this->scoreColor = 'green';
    }

    public function decrHealth() {
        $this->health--;
        $this->healthTicks = Config::$statColorDuration;
        $this->healthColor = "red";
    }

    public function tick() {
        $this->distance++;

        if ($this->healthTicks > 0) {
            $this->healthTicks--;
        }

        if ($this->shotTicks > 0) {
            $this->shotTicks--;
        }

        if ($this->scoreTicks > 0) {
            $this->scoreTicks--;
        }
    }

    public function resetShots() {
        $this->shots = Config::$defaultShots;
        $this->shotTicks = Config::$statColorDuration;
        $this->shotColor = 'green';
    }

    public function decrShots() {
        if ($this->shots > 0) {
            $this->shots--;
        }

        $this->shotTicks = Config::$statColorDuration;
        $this->shotColor = "red";
    }

    public function hit() {

    }

    public function draw() {
        $ret = "Hits: " . $this->hits . " Misses: " . $this->misses . " / " . ($this->hits + $this->misses);

        return $ret;
    }
}