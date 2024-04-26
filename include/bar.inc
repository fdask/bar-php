<?php
class Bar {
    private $x;
    private $width;
    private $dir;
    private $lastScore;
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

    public function setLastScore($ls) {
        $this->lastScore = $ls;
    }

    public function getX() {
        return $this->x;
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

    public function moveLeft() {
        if ($this->x > 0) {
            $this->x--;
        } else {
            $this->dir = Bar::$dirRight;
            $this->x++;
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

    public function moveRight() {
        if ($this->x < $this->width - 1) {
            $this->x++;
        } else {
            $this->dir = Bar::$dirLeft;
            $this->x--;
        }
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
                        //echo "Door going up!\n";
                        $ret .= Config::arrowUp();
                    } else {
                        //echo "Door doing down!\n";
                        $ret .= Config::arrowDown();
                    }
                }
            } else if ($x == $this->x) {
                $ret .= "|";
            } else {
                $ret .= "-";
            }
        }

        if (!is_null($this->lastScore)) {
            $ret .= " [" . $this->lastScore . "%]";
        }

        return $ret;
    }
}
