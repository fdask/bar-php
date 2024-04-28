<?php
class Game {
    // save terminal settings to restore later
    private $term;

    // what bar # are we currently on
    private $bar;

    // the collection of bars
    private $bars;

    // main game timer
    private $ticks;

    // % from our last shot
    private $lastScore;

    // boolean
    private $doubleSpeed;

    // what bar # is the target on
    private $targetBar;

    // the player object
    private $player;

    // the level data object
    private $level;

    // bumpers
    private $leftBumper;
    private $rightBumper;

    public static $bumperDmg = 'DMG';
    public static $bumperNoop = 'NOOP';
    public static $bumperBase = 'BASE';

    public function __construct() {
        $this->term = `stty -g`;
		system("stty -icanon -echo");

        echo "\n";
       
        $this->level = new Level();
        $this->player = new Player();

        // populate the initial bar!
        $width = rand(Config::$minBarWidth, Config::$maxBarWidth);
        $speed = rand($this->level->minBarSpeed, $this->level->maxBarSpeed);

        $b = new Bar($width, $speed);
        $this->bars[] = $b;

        // populate the initial bumpers
        $this->leftBumper = Game::$bumperBase;
        $this->rightBumper = Game::$bumperNoop;

        // initialize some settings       
        $this->targetBar = 0;

        $this->bar = 0; 
        $this->ticks = 0;
        $this->doubleSpeed = false;

        // add the second bar
        $this->addBar();

        // add the initial target
        if ($this->level->targetSize > 1) {
            $this->addWideTarget($this->level->targetSize);
        } else {
            $this->addTarget();    
        }

        // enter the main loop
        $this->gameLoop();
    }

    public function reset() {
        // reset bars, add two new ones
        $this->bar = 0;
        $this->bars = array();

        $width = rand(Config::$minBarWidth, Config::$maxBarWidth);
        $speed = rand($this->level->minBarSpeed, $this->level->maxBarSpeed);

        $b = new Bar($width, $speed);
        $this->bars[] = $b;
        
        $this->addBar();

        // add initial bumpers
        $this->leftBumper = Game::$bumperBase;
        $this->rightBumper = Game::$bumperNoop;
        
        // add the initial target
        if ($this->level->targetSize > 1) {
            $this->addWideTarget($this->level->targetSize);
        } else {
            $this->addTarget();    
        }
    }

    public function addTarget($bar = null, $x = null) {
        if (is_null($bar)) {
            $bar = $this->bar;
        }

        if (is_null($x)) {
            $x = $this->bars[$bar]->getUnoccupiedX();
        }

        $t = new Target($x);

        $this->targetBar = $bar;
        $this->bars[$bar]->addContent($x, $t);
    }

    public function addWideTarget($width, $bar = null) {
        if (is_null($bar)) {
            $bar = $this->bar;
        }

        $xs = $this->bars[$bar]->getUnoccupiedXStretch($width);

        foreach ($xs as $x) {
            $this->addTarget($bar, $x);
        }
    }

    public function addBar() {
        $width = rand(Config::$minBarWidth, Config::$maxBarWidth);
        $speed = rand(Config::$minBarSpeed, Config::$maxBarSpeed);

        $bar = new Bar($width, $speed);
        $destX = rand(0, $width - 1);
        $origX = $this->bars[$this->bar]->getUnoccupiedX();

        // add a door
        $orig = count($this->bars) - 1;
        $dest = count($this->bars);

        $this->bars[] = $bar;

        $this->addDoor($orig, $dest, $origX, $destX);
    }

    public function setBar($b) {
        $this->bar = $b;

        // if we're at the upper limit, add a new bar
        if ($b == count($this->bars) - 1) {
            $this->addBar();
        }

        // set the left and right bumper
        if ($this->bar === 0) {
            $this->leftBumper = Game::$bumperBase;
        } else {
            if (rand(0, 1)) {
                $this->leftBumper = Game::$bumperDmg;
            } else {
                $this->leftBumper = Game::$bumperNoop;
            }
        }

        if (rand(0, 1)) {
            $this->rightBumper = Game::$bumperDmg;
        } else {
            $this->rightBumper = Game::$bumperNoop;
        }

        if ($this->level->rocks && empty($this->bars[$this->bar]->getRocks())) {
            // add a random rock to the level
            $this->bars[$this->bar]->addRandomRock();
        }
    }

    public function addDoor($origBar, $destBar, $origX, $destX) {
        if (isset($this->bars[$origBar]) && isset($this->bars[$destBar])) {
            $doorA = new Door($origBar, $origX, $destBar, $destX, Door::$dirUp);
            $doorB = new Door($destBar, $destX, $origBar, $origX, Door::$dirDown);
            $this->bars[$origBar]->addContent($origX, $doorA);
            $this->bars[$destBar]->addContent($destX, $doorB);
        } 
    }

    public function gameLoop() {
        $this->draw();
        
        stream_set_blocking(STDIN, false); // Do not wait

        // START OF INFINITE LOOP
        while (1) {
            $c = Config::waitForInput();

            switch ($c) {
                case ' ':
                    if ($this->player->shots > 0) {
                        // switch direction
                        $this->bars[$this->bar]->switchDir();

                        // if we're on a target, remove it
                        $curX = $this->bars[$this->bar]->getX();
                        $targets = $this->bars[$this->bar]->getTargets();
              
                        if (in_array($curX, $targets)) {
                            // HIT
                            $this->player->hit();
                            $this->level->hit();
                            
                            // calculate the score (how close to the center of the target, were we)
                            $this->player->addToScore($this->getTargetScore());
                            
                            // check to see if this clears the level
                            if ($this->level->targetProgress >= $this->level->targetCount) {
                                // level up
                                $this->level->levelUp();
 
                                // reset the level
                                $this->reset();
                            }

                            $this->bars[$this->bar]->clearTargets();
                           
                            // add a new target elsewheres
                            $randBar = rand(0, count($this->bars) - 1);
                            
                            if ($this->level->targetSize > 1) {
                                $xs = $this->bars[$randBar]->getUnoccupiedXStretch($this->level->targetSize);

                                foreach ($xs as $x) {
                                    $this->addTarget($randBar, $x);
                                }
                            } else {
                                $randX = $this->bars[$randBar]->getUnoccupiedX();
                            
                                $this->addTarget($randBar, $randX);
                            }
                        } else {
                            // MISS
                            $this->player->misses++;
                        }

                        $this->player->decrShots();
                    } else {
                        // player tried to fire but has no shots left
                        $this->player->decrShots();
                    }

                    break;
                case Config::keypressUpArrow():
                case Config::keypressDownArrow():
                    // check to see if we hit a doorway
                    if ($this->bars[$this->bar]->onDoor()) {
                        $door = $this->bars[$this->bar]->getContents();
                        $dest = $door->getDest();
                        $destX = $door->getDestX();
                        $dir = $door->getDir();

                        if (
                            ($dir == Door::$dirUp && $c == Config::keypressUpArrow()) ||
                            ($dir == Door::$dirDown && $c == Config::keypressDownArrow())
                        ) {
                            // go through the door
                            $oldbar = $this->bar;
                            $olddir = $this->bars[$this->bar]->getDir();

                            // go to the $dest bar
                            $this->setBar($dest);

                            // we need to figure out whee we're coming out
                            $this->bars[$this->bar]->setX($destX);
                            $this->bars[$this->bar]->setDir($olddir);
                        }
                    } 
                
                    break;
                case Config::keypressLeftArrow():
                    // go left if we're going right already
                    if ($this->bars[$this->bar]->getDir() == Bar::$dirRight) {
                        $this->bars[$this->bar]->switchDir();
                    }

                    break;
                case Config::keypressRightArrow():
                    // go right if we're going left already
                    if ($this->bars[$this->bar]->getDir() == Bar::$dirLeft) {
                        $this->bars[$this->bar]->switchDir();
                    }

                    break;
                case 'c':
                    // clear out the marks
                    
                    break;
                case Config::keypressShiftRightArrow():
                    if ($this->bars[$this->bar]->getDir() == Bar::$dirRight) {
                        $this->doubleSpeed = true;
                    } else {
                        $this->bars[$this->bar]->switchDir();
                    }

                    break;
                case Config::keypressShiftLeftArrow():
                    if ($this->bars[$this->bar]->getDir() == Bar::$dirLeft) {
                        $this->doubleSpeed = true;
                    } else {
                        $this->bars[$this->bar]->switchDir();
                    }

                    break;
                case 'd':
                    // doubleTime!
                    $this->doubleSpeed = true;

                    break;
                case 'q':
                    // quit the game
                    $this->gameOver();

                    // just in case
                    exit;
                default:
                    break;
            }

            // TICK STUFF
            $this->ticks++;

            $x = $this->bars[$this->bar]->getX();
            $speed = $this->bars[$this->bar]->getSpeed();
            $rocks = $this->bars[$this->bar]->getRocks();
            $dir = $this->bars[$this->bar]->getDir();

            if ($this->doubleSpeed) {
                if ($dir == Bar::$dirLeft) {
                    if (in_array($x - 1, $rocks)) {
                        // destroy the rock
                        $this->bars[$this->bar]->clearRocks();
                    } 
                } else {
                    if (in_array($x + 1, $rocks)) {
                        $this->bars[$this->bar]->clearRocks();
                    }
                }

                $speed = floor($speed / 2);
            }

            if ($this->ticks >= $speed) {
                $this->ticks = 0;
                $this->level->tick();
                $this->player->tick();
        
                $this->bars[$this->bar]->tick();
                
                // if we hit an edge
                if ($this->bars[$this->bar]->onEdge()) {
                    $this->doubleSpeed = false;

                    // check what type of bumper we've hit
                    if ($this->bars[$this->bar]->getX() == 0) {
                        if ($this->leftBumper == Game::$bumperDmg) {
                            $this->decrHealth();
                        } else if ($this->leftBumper == Game::$bumperBase) {
                            $this->player->resetShots();
                        }
                    } else {
                        if ($this->rightBumper == Game::$bumperDmg) {
                            $this->decrHealth();
                        }
                    }
                }

                $this->draw();
            }
        }
        // END OF INFINITE LOOP
    }

    public function decrHealth() {
        $this->player->decrHealth();

        if ($this->player->health <= 0) {
            echo "OUT OF HEALTH\n";

            $this->gameOver();
        }
    }

    public function gameOver() {
        echo "GAME OVER\n\n";

        echo $this->player->draw() . "\n";

        // quit the game
		system("stty " . $this->term);

		exit;
    }

    /** 
     * calculates a "score" as the percentage distance from 0 
     * 
     * utilizes the ticks between drawing to determine distance
     * travelled within a characters bounds
     */
    public function getScore() {
        $x = $this->bars[$this->bar]->getX();
        $width = $this->bars[$this->bar]->getWidth();

        $segment = 1 / $width;

        $ticks = $this->ticks;
        $tickInterval = $this->bars[$this->bar]->getSpeed();

        $percent = $ticks / $tickInterval;
        
        $percentDifference = $segment * ($ticks / $tickInterval);

        return round(($percentDifference + ($segment * $x)) * 100, 2);
    }

    public function getTargetScore() {
        // instead of a percentage of the whole board,
        // get a percentage of where we fall in the target
        $targets = $this->bars[$this->bar]->getTargets();
        $width = $this->bars[$this->bar]->getWidth();
        $x = $this->bars[$this->bar]->getX();

        $segment = 1 / $width;
        $leftLimit = round((min($targets) * $segment) * 100, 2);
        $rightLimit = round(((max($targets) + 1) * $segment) * 100, 2);

        $score = $this->getScore();
        $h = ($rightLimit - $leftLimit) / 2;
        $half = $leftLimit + $h;

        if ($score >= $half) {
            $delta = $score - $half;
        } else {
            $delta = $half - $score;
        }

        $distancePer = $delta / $h;

        $final = round(100 - (100 * $distancePer), 0);

        return $final;
    }

    public function draw() {
        // draw the left bumper
        switch ($this->leftBumper) {
            case Game::$bumperDmg:
                $d = Config::$bumperDmgChar;
                break;
            case Game::$bumperBase:
                $d = Config::$bumperBaseChar;
                break;
            case Game::$bumperNoop:
                $d = Config::$bumperNoopChar;
                break;
            default:
                // do nothing
        }

        // draw the bar
        $d .= $this->bars[$this->bar]->draw();

        // draw the right bumper
        switch ($this->rightBumper) {
            case Game::$bumperDmg:
                $d .= Config::$bumperDmgChar;
                break;
            case Game::$bumperBase:
                $d .= Config::$bumperBaseChar;
                break;
            case Game::$bumperNoop:
                $d .= Config::$bumperNoopChar;
                break;
            default:
                // do nothing
        }

        $d .= " b: " . ($this->bar + 1) . "/" . count($this->bars) . " (" . ($this->targetBar + 1) . ")";
        $d .= " h: " . (($this->player->healthTicks > 0) ? Colors::fg_color($this->player->healthColor, $this->player->health) : $this->player->health);
        $d .= " s: " . (($this->player->shotTicks > 0) ? Colors::fg_color($this->player->shotColor, $this->player->shots) : $this->player->shots);
        $d .= " l: " . (($this->level->levelTicks > 0) ? Colors::fg_color($this->level->levelColor, $this->level->level) : $this->level->level);
        
        $remaining = $this->level->targetCount - $this->level->targetProgress;
        $d .= " r: " . (($this->level->targetTicks > 0) ? Colors::fg_color($this->level->targetColor, $remaining) : $remaining);
        $d .= " score: " . (($this->player->scoreTicks > 0) ? Colors::fg_color($this->player->scoreColor, $this->player->score) : $this->player->score);
        
        $d .= "\n";

        Config::draw($d);
    }
}
