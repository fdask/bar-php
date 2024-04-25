<?php
class Bar {
    private $x;
    private $width;
    private $dir;
    private $marks;
    private $lastScore;

    public function __construct($width) {
        $this->x = 0;
        $this->width = $width;
        $this->dir = "RIGHT";
        $this->marks = array();
    }

    public function setLastScore($ls) {
        $this->lastScore = $ls;
    }

    public function clearMarks() {
        $this->marks = array();
    }

    public function addMark() {
        $this->marks[] = $this->x;
    }

    public function getX() {
        return $this->x;
    }

    public function setX($x) {
        $this->x = $x;
    }

    public function switchDir() {
        if ($this->dir == "RIGHT") {
            $this->dir = "LEFT";
        } else {
            $this->dir = "RIGHT";
        }
    }

    public function move() {
        if ($this->dir == "RIGHT") {
            $this->moveRight();
        } else {
            $this->moveLeft();
        }
    }

    public function moveLeft() {
        if ($this->x > 0) {
            $this->x--;
        } else {
            $this->dir = "RIGHT";
            $this->x++;
        }
    }

    public function moveRight() {
        if ($this->x < $this->width - 1) {
            $this->x++;
        } else {
            $this->dir = "LEFT";
            $this->x--;
        }
    }

    public function getDir() {
        return $this->dir;
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
            if ($x == $this->x) {
                $ret .= "|";
            } else if (in_array($x, $this->marks)) {
                $ret .= "m";
            } else {
                $ret .= "-";
            }
        }

        if (!is_null($this->lastScore)) {
            $ret .= " [" . $this->lastScore . "%]";
        }

        $ret .= "\n";

        return $ret;
    }
}
