<?php
class Bar {
    private $x;
    private $width;
    private $dir;
    private $speed;
    private $contents;

    public static $dirLeft = "LEFT";
    public static $dirRight = "RIGHT";

    public function __construct($width, $speed) {
        $this->x = 0;
        $this->width = $width;
        $this->speed = $speed;
        $this->dir = Bar::$dirRight;
    
        $this->contents = null;

        for ($x = 0; $x < $this->width; $x++) {
            $this->contents[$x] = null;
        }
    }

    public function clearTargets() {
        for ($x = 0; $x < count($this->contents); $x++) {
            if ($this->contents[$x] instanceof Target) {
                $this->contents[$x] = null;
            }
        }
    }

    public function addContent($x, $c) {
        $this->contents[$x] = $c;
    }

    public function getContents($x = null) {
        if (is_null($x)) {
            $x = $this->x;
        }

        return $this->contents[$x];
    }

    public function getSpeed() {
        return $this->speed;
    }

    public function setSpeed($s) {
        $this->speed = $s;
    }

    public function getX() {
        return $this->x;
    }

    public function getUnoccupiedX() {
        do {
            $x = rand(0, $this->width - 1);
        } while (!is_null($this->contents[$x]));

        return $x;
    }

    public function getUnoccupiedXStretch($width) {       
        // find a contiguous free space clump of $width
        do {
            $ret = array();

            $start = rand(0, ($this->width - 1 - $width));

            $occupied = false;

            for ($x = $start; $x < $start + $width; $x++) {
                if (!is_null($this->contents[$x])) {
                    $occupied = true;
                } else {
                    $ret[] = $x;
                }
            }
        } while ($occupied == true);

        return $ret;
    }

    public function setX($x) {
        $this->x = $x;
    }

    public function switchDir() {
        if ($this->dir == Bar::$dirRight) {
            $this->dir = Bar::$dirLeft;
        } else {
            $this->dir = Bar::$dirRight;
        }
    }

    public function move() {
        if ($this->dir == Bar::$dirRight) {
            $this->moveRight();
        } else {
            $this->moveLeft();
        }
    }

    public function getRocks() {
        $ret = array();

        for ($x = 0; $x < $this->width; $x++) {
            if ($this->contents[$x] instanceof Rock) {
                $ret[] = $x;
            }
        }

        return $ret;
    }

    public function getTargets() {
        $ret = array();

        for ($x = 0; $x < $this->width; $x++) {
            if ($this->contents[$x] instanceof Target) {
                $ret[] = $x;
            }
        }

        return $ret;
    }

    public function moveLeft() {
        if ($this->x > 0 && (!in_array(($this->x - 1), $this->getRocks()))) {
            $this->x--;
        } else {
            $this->dir = Bar::$dirRight;
            $this->x++;
        }
    }

    public function moveRight() {
        if ($this->x < $this->width - 1 && (!in_array(($this->x + 1), $this->getRocks()))) {
            $this->x++;
        } else {
            $this->dir = Bar::$dirLeft;
            $this->x--;
        }
    }

    public function onEdge() {
        if ($this->x == 0 || $this->x == ($this->width - 1)) {
            return true;
        }

        return false;
    }

    public function onDoor() {
        $c = $this->getContents($this->x);

        if (!is_null($c) && $c instanceof Door) {
            return true;
        }

        return false;
    }

    public function onTarget() {
        $c = $this->getContents($this->x);

        if (!is_null($c) && $c instanceof Target) {
            return true;
        }

        return false;
    }

    public function addRandomRock() {
        do {
            $x = rand(0, $this->width - 1);
        } while (!is_null($this->contents[$x]));

        $this->contents[$x] = new Rock($x);
    }

    public function getDir() {
        return $this->dir;
    }

    public function setDir($d) {
        $this->dir = $d;
    }

    public function getWidth() {
        return $this->width;
    }

    public function tick() {
        $this->move();
    }

    public function draw() {
        $ret = "";

        for ($x = 0; $x < $this->width; $x++) {
            $c = $this->getContents($x);
                        
            if (!is_null($c)) {
                if ($c instanceof Door) {
                    $dir = $c->getDir();

                    if ($dir == Door::$dirUp) {
                        $ret .= Config::arrowUp();
                    } else {
                        $ret .= Config::arrowDown();
                    }
                } else if ($c instanceof Rock) {
                    $ret .= "*";
                } else if ($c instanceof Target) {
                    $ret .= Colors::bg_color('green', '#');
                }
            } else if ($x == $this->x) {
                $ret .= "|";
            } else {
                $ret .= "-";
            }
        }

        return $ret;
    }
}
